#pragma once

#include "pch.h"

typedef struct controlSet {
    float moveSpeed;
    float turnSpeed;

    // NOTICE: No scale, that's for animation
    mat4 rotation;
    mat4 translation;

    void (*controlFunction)(struct controlSet *);
} ControlSet, *PControlSet;

extern uint32_t controlSetCount;
extern PControlSet controlSets;

void initializeWorld();
uint32_t initializeControlSet(float turnSpeed, float moveSpeed, void (*controlFunction)(PControlSet));
void bindControlSet(uint32_t controlSetIndex, uint32_t nodeIndex);
void updateControlSet(PControlSet controlSet);

// NOTICE: Declare your control functions here
void firstPersonControl(PControlSet controlSet);