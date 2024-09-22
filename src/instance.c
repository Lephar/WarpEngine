#include "instance.h"
#include "window.h"
#include "helper.h"

extern PFN_vkGetInstanceProcAddr getInstanceProcAddr;
extern const char *title;
extern uint32_t instanceExtensionCount;
extern const char **instanceExtensionNames;

VkInstance instance;
#ifndef NDEBUG
VkDebugUtilsMessengerEXT messenger;
#endif // NDEBUG

void createInstance() {
    uint32_t layerCount = 0;
    // Reserve extra space for Validation Layers just in case
    const char **layerNames = malloc((layerCount + 1) * sizeof(const char *));
    layerNames[layerCount] = NULL;
    
    uint32_t extensionCount = instanceExtensionCount;
    // Reserve extra space for Validation Layers extension just in case
    const char **extensionNames = malloc((extensionCount + 1) * sizeof(const char *));
    memcpy(extensionNames, instanceExtensionNames, extensionCount * sizeof(const char *));
    extensionNames[extensionCount] = NULL;
    
    void *instanceNext = NULL;

#ifndef NDEBUG
    layerNames[layerCount] = "VK_LAYER_KHRONOS_validation";
    layerCount++;

    extensionNames[extensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    extensionCount++;

    VkDebugUtilsMessengerCreateInfoEXT messengerInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = NULL,
        .flags = {},
        .messageSeverity =  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
        .messageType =  VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = messageCallback,
        .pUserData = NULL
    };

    instanceNext = &messengerInfo;
#endif // NDEBUG

    VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = title,
        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
        .pEngineName = title,
        .engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
        .apiVersion = VK_API_VERSION_1_3
    };

    VkInstanceCreateInfo instanceInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = instanceNext,
        .flags = {},
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = layerCount,
        .ppEnabledLayerNames = layerNames,
        .enabledExtensionCount = extensionCount,
        .ppEnabledExtensionNames = extensionNames
    };

    vkCreateInstance(&instanceInfo, NULL, &instance);
    
#ifndef NDEBUG
    PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)getInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    createDebugUtilsMessenger(instance, &messengerInfo, NULL, &messenger);
#endif // NDEBUG

    free(extensionNames);
    free(layerNames);
}

void desroyInstance() {
#ifndef NDEBUG
    PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)getInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    destroyDebugUtilsMessenger(instance, messenger, NULL);
#endif // NDEBUG

    vkDestroyInstance(instance, NULL);
}