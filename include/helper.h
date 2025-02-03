#pragma once

#include "pch.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif //PATH_MAX

#define  SEC_TO_MSEC  1000.0f
#define MSEC_TO_USEC  SEC_TO_MSEC
#define USEC_TO_NSEC MSEC_TO_USEC

#define EPSILON 0.000000000931322574615478515625f // 2 ^ -30

struct data {
    size_t size;
    char *content;
} typedef Data;

void debug(const char *fmt, ...);

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
                                               VkDebugUtilsMessageTypeFlagsEXT             type,
                                               const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                               void                                       *pUserData);

void *loadFunction(const char *name);

int32_t compareFloat(float first, float second);

float radians(float degrees);
