#pragma once

#include "pch.h"

#define  SEC_TO_MSEC      1000.0f
#define MSEC_TO_USEC  SEC_TO_MSEC
#define USEC_TO_NSEC MSEC_TO_USEC

#define EPSILON 0.000000000931322574615478515625f // 2 ^ -30

float radians(float degrees);

int32_t compareFloat(float first, float second);
uint32_t popcount(uint32_t value);

uint64_t align(uint64_t value, uint64_t alignment);
uint64_t alignBack(uint64_t value, uint64_t alignment);

int8_t bmin(int8_t first, int8_t second);
int8_t bmax(int8_t first, int8_t second);

uint8_t ubmin(uint8_t first, uint8_t second);
uint8_t ubmax(uint8_t first, uint8_t second);

int16_t smin(int16_t first, int16_t second);
int16_t smax(int16_t first, int16_t second);

uint16_t usmin(uint16_t first, uint16_t second);
uint16_t usmax(uint16_t first, uint16_t second);

int32_t imin(int32_t first, int32_t second);
int32_t imax(int32_t first, int32_t second);

uint32_t umin(uint32_t first, uint32_t second);
uint32_t umax(uint32_t first, uint32_t second);

int64_t lmin(int64_t first, int64_t second);
int64_t lmax(int64_t first, int64_t second);

uint64_t ulmin(uint64_t first, uint64_t second);
uint64_t ulmax(uint64_t first, uint64_t second);
