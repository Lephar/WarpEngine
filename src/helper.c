#include "helper.h"

extern char rootPath[];

extern PFN_vkGetInstanceProcAddr getInstanceProcAddr;
extern VkInstance instance;

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback( VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                VkDebugUtilsMessageTypeFlagsEXT type,
                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                void* pUserData) {
    (void)severity;
    (void)type;
    (void)pUserData;

    debug("%s", pCallbackData->pMessage);

    return VK_FALSE;
}

void *loadFunction(const char *name) {
    return getInstanceProcAddr(instance, name);
}

// Not the most optimal solution but gets the job done
uint32_t popcount(uint32_t value) {
    uint32_t count = 0;

    while(value) {
        count += value & 1;
        value >>= 1;
    }

    return count;
}

// TODO: Change to iterative, no need for function call overhead
uint32_t byte_to_binary(uint8_t value) {
    return (value == 0 || value == 1 ? value : ((value % 2) + 10 * byte_to_binary(value / 2)));
}

void readFile(const char *relativePath, uint32_t binary, size_t *size, char **data) {
    char fullPath[PATH_MAX];
    sprintf(fullPath, "%s/%s", rootPath, relativePath);

    FILE *file = fopen(fullPath, binary ? "rb" : "r");
    fseek(file, 0, SEEK_END);
    *size = ftell(file) + (binary ? 0 : 1);
    rewind(file);

    *data = malloc(*size);
    fread(*data, 1, *size, file);
    fclose(file);

    // TODO: Is this necessary?
    if(!binary) {
        (*data)[*size - 1] = '\0';
    }
}
