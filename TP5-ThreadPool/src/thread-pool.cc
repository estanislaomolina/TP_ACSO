/**
 * File: thread-pool.cc
 * --------------------
 * Presents the implementation of the ThreadPool class.
 */

#include "thread-pool.h"
#include <stdexcept>
using namespace std; // std:: es el namespace donde se encuentra la librería estándar de C++

ThreadPool::ThreadPool(size_t numThreads) : wts(numThreads), done(false)
{
    // Constructor que configura el Threadpool para crear un número especificado de hilos.
    for (size_t i = 0; i < numThreads; i++)
    {
        idWorker.push(i);              // inicializo el vector de hilos disponibles
        idWorker_disponibles.signal(); // incremento el semaphore de workers
    }
    // Inicializo los hilos de trabajo
    for (size_t i = 0; i < numThreads; i++)
    {
        wts[i].idWorker = i;                              // asigno el idWorker al worker
        wts[i].ts = thread(&ThreadPool::worker, this, i); // inicio el worker con su id
    }
    dt = thread(&ThreadPool::dispatcher, this); // inicio el dispatcher
}

void ThreadPool::schedule(const function<void(void)> &thunk)
{
    // Programa el thunk proporcionado para ser ejecutado por alguno de los hilos del ThreadPool
    // tan pronto como todos los thunks previamente programados hayan sido manejados.
    if (done)
    {
        throw runtime_error("El ThreadPool ya ha sido destruido."); // lanzo una excepción si el ThreadPool ya ha sido destruido
    }
    if (!thunk)
    {
        throw std::invalid_argument("El thunk no puede ser nulo."); // lanzo una excepción si el thunk es nulo
    }
    {
        std::lock_guard<mutex> lock(queueLock); // bloqueo el mutex para proteger el acceso a la queue de tareas
        tareas.push(thunk);                     // agrego el thunk a la queue de tareas
    }

    {
        std::lock_guard<mutex> lock(completionMutex); // bloqueo el mutex para proteger el acceso a las variables de finalización
        totalTareas++;                                // incremento el contador de tareas totales
    }
    tareas_disponibles.signal(); // incremento el semaphore de tareas disponibles
}

void ThreadPool::wait()
{
    // Bloquea y espera hasta que todos los thunks previamente programados hayan sido ejecutados completamente
    std::unique_lock<std::mutex> lock(completionMutex); // bloqueo el mutex para proteger el acceso a las variables de finalización
    completionCv.wait(lock, [this]() {                  //
        return tareasCompletadas == totalTareas;
    });
}

void ThreadPool::worker(int id)
{
    // definicion del worker con sus distintas tareas
    while (true)
    {
        wts[id].sem.wait(); // espero a que haya una tarea disponible
        if (done)
        {
            return; // si se ha marcado como terminado, salgo del worker
        }
        wts[id].thunk(); // ejecuto la tarea
        {
            std::lock_guard<std::mutex> lock(idWorkerLock); // bloqueo el mutex para proteger el acceso a las variables de finalización
            idWorker.push(id);                              // agrego el id del worker a la queue de workers disponibles
        }
        idWorker_disponibles.signal(); // incremento el semaphore de workers
        {
            std::lock_guard<std::mutex> lock(completionMutex); // bloqueo el mutex para proteger el acceso a las variables de finalización
            tareasCompletadas++;                               // incremento el contador de tareas completadas
            if (tareasCompletadas == totalTareas)
            {
                completionCv.notify_all(); // notifico a todos los que están esperando que todas las tareas se han completado
            }
        }
    }
}

void ThreadPool::dispatcher()
{
    // defino el dispatcher que se encarga de asignar las tareas a los workers disponbles
    while (true)
    {
        tareas_disponibles.wait(); // espero a que haya tareas disponibles
        {
            std::lock_guard<std::mutex> lock(queueLock); // bloqueo el mutex para proteger el acceso a la queue de tareas
            if (done && tareas.empty())
            {
                return; // si se ha marcado como terminado y no hay tareas, salgo del dispatcher
            }
        }
        function<void(void)> tarea;
        {
            std::lock_guard<std::mutex> lock(queueLock); // bloqueo el mutex para proteger el acceso a la queue de tareas
            if (!tareas.empty())
            {
                tarea = tareas.front(); // obtengo la tarea del frente de la queue
                tareas.pop();           // saco la tarea de la queue
            }
        }
        idWorker_disponibles.wait(); // espero a que haya un worker disponible
        int workerId;
        {
            std::lock_guard<std::mutex> lock(idWorkerLock); // bloqueo el mutex para proteger el acceso a la queue de workers disponibles
            if (!idWorker.empty())
            {
                workerId = idWorker.front(); // obtengo el id del worker disponible
                idWorker.pop();              // saco el worker de la queue
            }
            else
            {
                continue; // si no hay workers disponibles, sigo esperando
            }
        }
        if (tarea)
        {                                    // si hay una tarea, la asigno al worker
            wts[workerId].thunk = tarea;     // asigno la tarea al worker
            wts[workerId].available = false; // marco el worker como no disponible
            wts[workerId].sem.signal();      // despierto al worker para que ejecute la tarea
        }
    }
}

ThreadPool::~ThreadPool()
{
    //  Destructor que espera a que todos los thunks programados se ejecuten y luego cierra
    // correctamente el ThreadPool y cualquier recurso utilizado durante su vida útil.
    wait();                      // espero a que todas las tareas se completen
    done = true;                 // marco el ThreadPool como terminado
    tareas_disponibles.signal(); // despierto al dispatcher para que salga del bucle
    if (dt.joinable())
    {
        dt.join(); // espero a que el dispatcher termine
    }
    for (size_t i = 0; i < wts.size(); i++)
    {
        wts[i].sem.signal(); // despierto al worker para que salga del bucle
    }
    for (size_t i = 0; i < wts.size(); i++)
    {
        if (wts[i].ts.joinable())
        {
            wts[i].ts.join(); // espero a que el worker termine
        }
    }
}
