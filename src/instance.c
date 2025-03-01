#include "instance.h"
#include "helper.h"

extern char executableName[];

extern uint32_t requiredInstanceExtensionCount;
extern const char **requiredInstanceExtensionNames;

VkInstance instance;
#if DEBUG
VkDebugUtilsMessengerEXT messenger;
#endif

void createInstance() {
    uint32_t layerCount = 0;
    // Reserve extra space for Validation Layers just in case
    const char **layerNames = malloc((layerCount + 1) * sizeof(const char *));
    layerNames[layerCount] = NULL;

    uint32_t extensionCount = requiredInstanceExtensionCount;
    // Reserve extra space for Validation Layers extension just in case
    const char **extensionNames = malloc((extensionCount + 1) * sizeof(const char *));
    memcpy(extensionNames, requiredInstanceExtensionNames, extensionCount * sizeof(const char *));
    extensionNames[extensionCount] = NULL;

    void *instanceNext = NULL;

#if DEBUG
    layerNames[layerCount] = "VK_LAYER_KHRONOS_validation";
    layerCount++;

    extensionNames[extensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    extensionCount++;

    VkDebugUtilsMessengerCreateInfoEXT messengerInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = NULL,
        .flags = 0,
        .messageSeverity =  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT   |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
        .messageType =  VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = messageCallback,
        .pUserData = NULL
    };

    instanceNext = &messengerInfo;
#endif

    VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = executableName,
        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
        .pEngineName = executableName,
        .engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
        .apiVersion = VK_API_VERSION_1_3
    };

    VkInstanceCreateInfo instanceInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = instanceNext,
        .flags = 0,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = layerCount,
        .ppEnabledLayerNames = layerNames,
        .enabledExtensionCount = extensionCount,
        .ppEnabledExtensionNames = extensionNames
    };

    debug("Instance layers (count = %d):", layerCount);
    for(uint32_t index = 0; index < layerCount; index++) {
        debug("\t%s", layerNames[index]);
    }

    debug("Instance extensions (count = %d):", extensionCount);
    for(uint32_t index = 0; index < extensionCount; index++) {
        debug("\t%s", extensionNames[index]);
    }

    vkCreateInstance(&instanceInfo, NULL, &instance);
    debug("Instance created");

#if DEBUG
    PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessenger = loadFunction("vkCreateDebugUtilsMessengerEXT");
    assert(createDebugUtilsMessenger);

    createDebugUtilsMessenger(instance, &messengerInfo, NULL, &messenger);
    debug("Messenger created");
#endif

    free(extensionNames);
    free(layerNames);
}

void destroyInstance() {
#if DEBUG
    PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessenger = loadFunction("vkDestroyDebugUtilsMessengerEXT");
    assert(destroyDebugUtilsMessenger);

    destroyDebugUtilsMessenger(instance, messenger, NULL);
    debug("Messenger destroyed");
#endif

    vkDestroyInstance(instance, NULL);
    debug("Instance destroyed");
}
