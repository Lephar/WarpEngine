#pragma once

#include "pch.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif //PATH_MAX

#define  SEC_TO_MSEC  1000.0f
#define MSEC_TO_USEC  SEC_TO_MSEC
#define USEC_TO_NSEC MSEC_TO_USEC

#define EPSILON 0.000000000931322574615478515625f // 2 ^ -30
/*
#define debug(...) \
    do { \
        if(DEBUG) { \
            struct timespec timespec; \
            clock_gettime(CLOCK_REALTIME, &timespec); \
            \
            char timestamp[INT8_MAX]; \
            strftime(timestamp, INT8_MAX, "%Y-%m-%d %H:%M:%S", localtime((time_t *) &timespec.tv_sec)); \
            \
            printf("%s %7.3f %s:%u:%s():\t", timestamp, timespec.tv_nsec / (MSEC_TO_USEC * USEC_TO_NSEC), __FILE__, __LINE__, __func__); \
            printf(__VA_ARGS__); \
            printf("\n"); \
        } \
    } while (0)
*/
void debug(const char *fmt, ...);

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
                                               VkDebugUtilsMessageTypeFlagsEXT             type,
                                               const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                               void                                       *pUserData);

void *loadFunction(const char *name);

uint32_t popcount(uint32_t value);
uint32_t binarize(uint8_t decimal);

int32_t compareFloat(float first, float second);
float radians(float degrees);

// WARN: Free the output data!
void readFile(const char *relativePath, uint32_t binary, size_t *size, char **data);
