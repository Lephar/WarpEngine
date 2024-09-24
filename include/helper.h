#include "zero.h"

#ifndef NDEBUG
SDL_LogPriority convertLogSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT severity);

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback( VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                VkDebugUtilsMessageTypeFlagsEXT type,
                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                void* pUserData);
#endif // NDEBUG

uint32_t popcount(uint32_t value);
