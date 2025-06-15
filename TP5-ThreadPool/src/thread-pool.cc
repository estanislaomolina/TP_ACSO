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
    return;
}

void ThreadPool::dispatcher(){
    return;
}



ThreadPool::~ThreadPool() {
    //  Destructor que espera a que todos los thunks programados se ejecuten y luego cierra 
    // correctamente el ThreadPool y cualquier recurso utilizado durante su vida útil.
    return;
}
