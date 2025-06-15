/**
 * File: thread-pool.cc
 * --------------------
 * Presents the implementation of the ThreadPool class.
 */

#include "thread-pool.h"
using namespace std;

ThreadPool::ThreadPool(size_t numThreads) : wts(numThreads), done(false) {
    // Constructor que configura el Threadpool para crear un número especificado de hilos.
    for (size_t i = 0; i < numThreads; i++ ) {
        idWorker.push(i); // inicializo el vector de hilos disponibles
        idWorker_disponibles.signal(); // incremento el semaphore de workers
    }
    // Inicializo los hilos de trabajo
    for (size_t i = 0; i < numThreads; i++) {
        wts[i].idWorker = i; // asigno el idWorker al worker
        wts[i].ts = thread(&ThreadPool::worker, this, i); // inicio el worker con su id
}
    dt = thread(&ThreadPool::dispatcher, this); // inicio el dispatcher
}


void ThreadPool::schedule(const function<void(void)>& thunk) {
    // Programa el thunk proporcionado para ser ejecutado por alguno de los hilos del ThreadPool
    // tan pronto como todos los thunks previamente programados hayan sido manejados.
    {
        std::lock_guard<mutex> lock(queueLock); // bloqueo el mutex para proteger el acceso a la queue de tareas
        tareas.push(thunk); // agrego el thunk a la queue de tareas
    }

    {
    std::lock_guard<mutex> lock(completionMutex); // bloqueo el mutex para proteger el acceso a las variables de finalización
    totalTareas++; // incremento el contador de tareas totales
    }
    tareas_disponibles.signal(); // incremento el semaphore de tareas disponibles
}

void ThreadPool::wait() {
    // Bloquea y espera hasta que todos los thunks previamente programados hayan sido ejecutados completamente
    std::unique_lock<std::mutex> lock(completionMutex); // bloqueo el mutex para proteger el acceso a las variables de finalización
    completionCv.wait(lock, [this]() { // 
      return tareasCompletadas == totalTareas;
    });
}

void ThreadPool::worker(int id){
    // definicion del worker con sus distintas tareas
    while (true) {
        idWorker_disponibles.wait(); // espera a que haya un worker disponible
        idWorkerLock.lock(); // bloqueo el mutex para proteger el acceso a la queue de workers disponibles
        if (done && tareas.empty()) {
            idWorkerLock.unlock(); // desbloqueo el mutex
            return; // si se ha marcado como terminado y no hay tareas, salgo del worker
        }
        int workerId = idWorker.front(); // obtengo el id del worker disponible
        idWorker.pop(); // saco el worker de la queue
        idWorkerLock.unlock(); // desbloqueo el mutex

        function<void(void)> tarea;
        tareas_disponibles.wait(); // espero a que haya tareas disponibles

        {
            std::lock_guard<mutex> lock(queueLock); // bloqueo el mutex para proteger el acceso a la queue de tareas
            if (!tareas.empty()) {
                tarea = tareas.front(); // obtengo la tarea del frente de la queue
                tareas.pop(); // saco la tarea de la queue
            }
        }

        if (tarea) { // si hay una tarea, la ejecuto
            tarea();
            wts[workerId].sem.signal(); // despierto al worker correspondiente
            std::lock_guard<std::mutex> lock(completionMutex); // bloqueo el mutex para proteger el acceso a las variables de finalización
            tareasCompletadas++; // incremento el contador de tareas completadas
            completionCv.notify_all(); // notifico a todos los que están esperando en wait()
        }
    }
}

void ThreadPool::dispatcher(){
    // defino el dispatcher que se encarga de asignar las tareas a los workers disponbles
    while (true) {
        tareas_disponibles.wait(); // espera a que haya tareas disponibles
        if (done && tareas.empty()) {
            return; // si se ha marcado como terminado y no hay tareas, salgo del dispatcher
        }

        idWorker_disponibles.wait(); // espera a que haya un worker disponible
        idWorkerLock.lock(); // bloqueo el mutex para proteger el acceso a la queue de workers disponibles
        int workerId = idWorker.front(); // obtengo el id del worker disponible
        idWorker.pop(); // saco el worker de la queue
        idWorkerLock.unlock(); // desbloqueo el mutex

        wts[workerId].sem.signal(); // despierto al worker correspondiente
    }
}



ThreadPool::~ThreadPool() {
    //  Destructor que espera a que todos los thunks programados se ejecuten y luego cierra 
    // correctamente el ThreadPool y cualquier recurso utilizado durante su vida útil.
    return;
}
