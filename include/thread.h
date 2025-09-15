#pragma once

#include "pch.h"

typedef struct thread {
    pthread_t thread;

    pthread_mutex_t mutex;
    pthread_cond_t cond;

    uint32_t dependencyCount;
    struct thread **dependencies;

    void (*routine)();
} Thread, *PThread;

PThread dispatchThread(uint32_t dependencyCount, PThread *dependencies, void (*routine)());
void waitThread(PThread thread);
