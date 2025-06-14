/**
 * File: thread-pool.cc
 * --------------------
 * Presents the implementation of the ThreadPool class.
 */

#include "thread-pool.h"
using namespace std;

ThreadPool::ThreadPool(size_t numThreads) : wts(numThreads), done(false) {
    // Constructor que configura el Threadpool para crear un número especificado de hilos.
}


void ThreadPool::schedule(const function<void(void)>& thunk) {
    // Programa el thunk proporcionado para ser ejecutado por alguno de los hilos del ThreadPool
    // tan pronto como todos los thunks previamente programados hayan sido manejados.
}


void ThreadPool::wait() {
    // Bloquea y espera hasta que todos los thunks previamente programados hayan sido ejecutados completamente
}

void ThreadPool::worker(int id){}

void ThreadPool::dispatcher(){}



ThreadPool::~ThreadPool() {
    //  Destructor que espera a que todos los thunks programados se ejecuten y luego cierra 
    // correctamente el ThreadPool y cualquier recurso utilizado durante su vida útil.

}
