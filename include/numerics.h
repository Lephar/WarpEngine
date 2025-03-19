#pragma once

#include "pch.h"

#define  SEC_TO_MSEC  1000.0f
#define MSEC_TO_USEC  SEC_TO_MSEC
#define USEC_TO_NSEC MSEC_TO_USEC

#define EPSILON 0.000000000931322574615478515625f // 2 ^ -30

int32_t compareFloat(float first, float second);

float radians(float degrees);
