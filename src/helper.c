#include "helper.h"

extern char path[];
extern char name[];

extern VkExtent2D extent;

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

// Not the most optimal solution but gets the job done
uint32_t popcount(uint32_t value) {
    uint32_t count = 0;

    while(value) {
        count += value & 1;
        value >>= 1;
    }

    return count;
}

uint32_t byte_to_binary(uint8_t value) {
    return (value == 0 || value == 1 ? value : ((value % 2) + 10 * byte_to_binary(value / 2)));
}

void parseArguments(int argc, char *argv[]) {
    debug("argc: %d", argc);
    for(int32_t argn = 0; argn < argc; argn++) {
        debug("\targv[%d]: %s", argn, argv[argn]);
    }

    debug("PATH_MAX: %d", PATH_MAX);

    const char * separator = strrchr(argv[0], '/');

    if(separator == NULL) {
        getcwd(path, PATH_MAX);
        strncpy(name, argv[0], PATH_MAX);
    } else {
        strncpy(path, argv[0], separator - argv[0]);
        strncpy(name, separator + 1, PATH_MAX);
    }

    path[PATH_MAX - 1] = '\0';
    name[PATH_MAX - 1] = '\0';

    debug("Path:  %s", path);
    debug("Name:  %s", name);

    if(argc >= 3) {
        extent.width  = atoi(argv[1]);
        extent.height = atoi(argv[2]);
    } else {
        extent.width  = 800;
        extent.height = 600;
    }

    debug("Width:  %d", extent.width);
    debug("Height: %d", extent.height);
}
