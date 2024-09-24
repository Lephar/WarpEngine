#include "helper.h"

#ifndef NDEBUG
SDL_LogPriority convertLogSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
    if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        return SDL_LOG_PRIORITY_ERROR;
    else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        return SDL_LOG_PRIORITY_WARN;
    else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        return SDL_LOG_PRIORITY_INFO;
    else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        return SDL_LOG_PRIORITY_VERBOSE;
    else
        return SDL_LOG_PRIORITY_DEBUG;
}

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback( VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                VkDebugUtilsMessageTypeFlagsEXT type,
                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                void* pUserData) {
    (void)type;
    (void)pUserData;

    SDL_LogMessage(SDL_LOG_CATEGORY_RENDER, convertLogSeverity(severity), "\tValidation Layers:\t%s\n", pCallbackData->pMessage);

    return VK_FALSE;
}
#endif // NDEBUG

// Not the most optimal solution but gets the job done
uint32_t popcount(uint32_t value) {
    uint32_t count = 0;

    while(value) {
        count += value & 1;
        value >>= 1;
    }

    return count;
}
