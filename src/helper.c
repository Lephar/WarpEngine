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

// TODO: This will be added in the C23 version too
uint32_t binarize(uint8_t decimal) {
    uint32_t binary = 0;
    uint8_t shift = CHAR_BIT - 1;
    uint8_t mask = 1 << shift;

    for(int i = 0; i < CHAR_BIT; i++) {
        binary = (binary * 10) + ((decimal & mask) >> shift);
        decimal <<= 1;
    }

    return binary;
}

int32_t compareFloat(float first, float second) {
    float difference = first - second;

    return (difference > EPSILON) - (difference < -EPSILON);
}

float radians(float degrees) {
    return M_PI * degrees / 180.0f;
}

void readFile(const char *relativePath, uint32_t binary, size_t *size, char **data) {
    char fullPath[PATH_MAX];
    sprintf(fullPath, "%s/%s", rootPath, relativePath);

    FILE *file = fopen(fullPath, binary ? "rb" : "r");
    fseek(file, 0, SEEK_END);
    *size = ftell(file) + (binary ? 0 : 1);
    rewind(file);

    *data = malloc(*size);
    size_t length = fread(*data, 1, *size, file);
    assert(length + (binary ? 0 : 1) == *size);
    fclose(file);

    // TODO: Is this necessary?
    if(!binary) {
        (*data)[*size - 1] = '\0';
    }
}
