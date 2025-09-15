#pragma once

#include "pch.h"

typedef struct thread {
    pthread_t thread;

    sem_t semaphore;

    uint32_t dependencyCount;
    struct thread **dependencies;

    void (*routine)();
} Thread, *PThread;

void waitThread(PThread thread);
PThread dispatchThread(uint32_t dependencyCount, PThread *dependencies, void (*routine)());
