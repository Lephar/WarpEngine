#pragma once

#include "pch.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif //PATH_MAX

#define debug(...) \
    do { \
        if (DEBUG) { \
            /*struct timespec timespec; \
            clock_gettime(CLOCK_REALTIME, &timespec); \
            char timestamp[20]; \
            strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", localtime(&(time_t){timespec.tv_sec})); \
            printf("%s %7.3Lf %s:%d:%s():\t", timestamp, timespec.tv_nsec / 1e6L, __FILE__, __LINE__, __func__); */\
            printf(__VA_ARGS__); \
            printf("\n"); \
        } \
    } while (0)

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback( VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                VkDebugUtilsMessageTypeFlagsEXT type,
                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                void* pUserData);

void *loadFunction(const char *name);

uint32_t popcount(uint32_t value);
uint32_t byte_to_binary(uint8_t value);

// WARN: Free the output data!
void readFile(const char *relativePath, uint32_t binary, size_t *size, char **data);
