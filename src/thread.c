#include "thread.h"

void *startThread(void *arg) {
    Thread *thread = arg;

    for(uint32_t dependencyIndex = 0; dependencyIndex < thread->dependencyCount; dependencyIndex++) {
        PThread dependency = thread->dependencies[dependencyIndex];

        pthread_mutex_lock(&dependency->mutex);
        pthread_cond_wait(&dependency->cond, &dependency->mutex);
        pthread_mutex_unlock(&dependency->mutex);
    }

    thread->routine();

    pthread_mutex_lock(&thread->mutex);
    pthread_cond_broadcast(&thread->cond);
    pthread_mutex_unlock(&thread->mutex);

    return nullptr;
}

PThread makeThread(uint32_t dependencyCount, PThread *dependencies, void (*routine)()) {
    PThread thread = malloc(sizeof(Thread));

    pthread_mutex_init(&thread->mutex, nullptr);
    pthread_cond_init(&thread->cond, nullptr);

    thread->dependencyCount = dependencyCount;
    thread->dependencies = malloc(thread->dependencyCount * sizeof(PThread));
    memcpy(thread->dependencies, dependencies, thread->dependencyCount * sizeof(PThread));

    thread->routine = routine;

    return thread;
}

PThread dispatchThread(uint32_t dependencyCount, PThread *dependencies, void (*routine)()) {
    PThread thread = makeThread(dependencyCount, dependencies, routine);

    pthread_create(&thread->thread, nullptr, startThread, thread);

    return thread;
}

void waitThread(PThread thread) {
    pthread_mutex_lock(&thread->mutex);
    pthread_cond_wait(&thread->cond, &thread->mutex);
    pthread_mutex_unlock(&thread->mutex);
}
