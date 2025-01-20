#include "helper.h"

extern char rootPath[];

extern PFN_vkGetInstanceProcAddr getInstanceProcAddr;
extern VkInstance instance;

void debug(const char *fmt, ...) {
    if(DEBUG) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        printf("\n");
        va_end(args);
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback( VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                VkDebugUtilsMessageTypeFlagsEXT type,
                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                void* pUserData) {
    //(void)severity;
    (void)type;
    (void)pUserData;

    if(severity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        debug("%s", pCallbackData->pMessage);

    return VK_FALSE;
}

void *loadFunction(const char *name) {
    return getInstanceProcAddr(instance, name);
}

int32_t compareFloat(float first, float second) {
    float difference = first - second;

    return (difference > EPSILON) - (difference < -EPSILON);
}

float radians(float degrees) {
    return M_PI * degrees / 180.0f;
}
