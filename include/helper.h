#include "zero.h"

SDL_LogPriority convertLogSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT severity);
VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                               VkDebugUtilsMessageTypeFlagsEXT type,
                                               const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                               void* pUserData);
