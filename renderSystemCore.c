#include "platformBase.h"
#include "renderSystemCore.h"

#include <stdio.h>
#include <string.h>

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                               VkDebugUtilsMessageTypeFlagsEXT type,
                                               const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                               void *pUserData) {
    (void) type;
    (void) pUserData;

    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        printf("%s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT getMessengerInfo(void) {
    return (VkDebugUtilsMessengerCreateInfoEXT) {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = messageCallback
    };
}

VkInstance createInstance(void) {
    uint32_t contextExtensionCount;
    const char **contextExtensionNames = getExtensions(&contextExtensionCount);

    uint32_t layerCount = 1u, extensionCount = contextExtensionCount + 1;
    const char *layerNames[] = {"VK_LAYER_KHRONOS_validation"}, *extensionNames[extensionCount];
    memcpy(extensionNames, contextExtensionNames, (extensionCount - 1) * sizeof(const char *));
    extensionNames[extensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

    VkDebugUtilsMessengerCreateInfoEXT messengerInfo = getMessengerInfo();

    VkApplicationInfo applicationInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Zero",
            .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
            .pEngineName = "Zero Engine",
            .engineVersion = VK_MAKE_VERSION(0, 1, 0),
            .apiVersion = VK_API_VERSION_1_2
    };

    VkInstanceCreateInfo instanceInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = layerCount,
            .ppEnabledLayerNames = layerNames,
            .enabledExtensionCount = extensionCount,
            .ppEnabledExtensionNames = extensionNames,
            .pNext = &messengerInfo
    };

    VkInstance instance;
    vkCreateInstance(&instanceInfo, NULL, &instance);
    return instance;
}

VkDebugUtilsMessengerEXT createMessenger(VkInstance instance) {
    VkDebugUtilsMessengerEXT messenger;
    VkDebugUtilsMessengerCreateInfoEXT messengerInfo = getMessengerInfo();
    PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessenger = getMessengerCreator(instance);
    createDebugUtilsMessenger(instance, &messengerInfo, NULL, &messenger);
    return messenger;
}

zrCore createCore(void) {
    zrCore core;
    core.instance = createInstance();
    core.messenger = createMessenger(core.instance);
    core.surface = createSurface(core.instance);
    return core;
}

void destroyCore(zrCore core) {
    PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessenger = getMessengerDestroyer(core.instance);

    vkDestroySurfaceKHR(core.instance, core.surface, NULL);
    destroyDebugUtilsMessenger(core.instance, core.messenger, NULL);
    vkDestroyInstance(core.instance, NULL);
}
