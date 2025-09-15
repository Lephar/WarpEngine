#include "thread.h"

void waitThread(PThread thread) {
    sem_wait(&thread->semaphore);
    sem_post(&thread->semaphore); // TODO: Use pthread_cond_wait maybe?
}

void *startThread(void *arg) {
    Thread *thread = arg;

    for(uint32_t dependencyIndex = 0; dependencyIndex < thread->dependencyCount; dependencyIndex++) {
        PThread dependency = thread->dependencies[dependencyIndex];
        waitThread(dependency);
    }

    thread->routine();
    sem_post(&thread->semaphore);

    return nullptr;
}

PThread makeThread(uint32_t dependencyCount, PThread *dependencies, void (*routine)()) {
    PThread thread = malloc(sizeof(Thread));

    sem_init(&thread->semaphore, 0, 0);

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
