#include "instance.h"

#include "window.h"

#include "file.h"
#include "logger.h"

VkInstance instance;
PFN_vkGetInstanceProcAddr instanceFunctionLoader;
#if DEBUG
VkDebugUtilsMessengerEXT messenger;

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    (void)severity;
    (void)type;
    (void)pUserData;

    debug("%s", pCallbackData->pMessage);

    return VK_FALSE;
}
#endif

void *loadInstanceFunction(const char *name) {
    void  *instanceFunction = instanceFunctionLoader(instance, name);
    assert(instanceFunction);
    return instanceFunction;
}

void createInstance() {
#if DEBUG
    const char *validationLayer = "VK_LAYER_KHRONOS_validation";
#endif

    const char *layers[] = {
#if DEBUG
        validationLayer
#endif
    };

    const uint32_t layerCount = sizeof(layers) / sizeof(const char *);

    const char *baseExtensions[] = {
#if DEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_EXT_LAYER_SETTINGS_EXTENSION_NAME,
#endif
        VK_EXT_SURFACE_MAINTENANCE_1_EXTENSION_NAME,
        VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME
    };

    const uint32_t baseExtensionCount = sizeof(baseExtensions) / sizeof(const char *);

    const uint32_t extensionCount = systemExtensionCount + baseExtensionCount;
    const char **extensions = malloc(extensionCount * sizeof(const char *));

    memcpy(extensions,                      baseExtensions,   baseExtensionCount   * sizeof(const char *));
    memcpy(extensions + baseExtensionCount, systemExtensions, systemExtensionCount * sizeof(const char *));

    void *instanceNext = nullptr;

#if DEBUG
    VkDebugUtilsMessengerCreateInfoEXT messengerInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity =  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT   |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
        .messageType =  VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = messageCallback,
        .pUserData = nullptr
    };

    //VkBool32 valueTrue  = VK_TRUE;
    VkBool32 valueFalse = VK_FALSE;

    VkLayerSettingEXT settings[] = {
        {
            .pLayerName = validationLayer,
            .pSettingName = "validate_sync",
            .type = VK_LAYER_SETTING_TYPE_BOOL32_EXT,
            .valueCount = 1,
            .pValues = &valueFalse
        },
        {
            .pLayerName = validationLayer,
            .pSettingName = "gpuav_enable",
            .type = VK_LAYER_SETTING_TYPE_BOOL32_EXT,
            .valueCount = 1,
            .pValues = &valueFalse
        },
        {
            .pLayerName = validationLayer,
            .pSettingName = "validate_best_practices",
            .type = VK_LAYER_SETTING_TYPE_BOOL32_EXT,
            .valueCount = 1,
            .pValues = &valueFalse
        },
        {
            .pLayerName = validationLayer,
            .pSettingName = "validate_best_practices_amd",
            .type = VK_LAYER_SETTING_TYPE_BOOL32_EXT,
            .valueCount = 1,
            .pValues = &valueFalse
        },
        {
            .pLayerName = validationLayer,
            .pSettingName = "validate_best_practices_nvidia",
            .type = VK_LAYER_SETTING_TYPE_BOOL32_EXT,
            .valueCount = 1,
            .pValues = &valueFalse
        }
    };

    uint32_t settingsCount = sizeof(settings) / sizeof(VkLayerSettingEXT);

    VkLayerSettingsCreateInfoEXT settingsInfo = {
        .sType = VK_STRUCTURE_TYPE_LAYER_SETTINGS_CREATE_INFO_EXT,
        .pNext = &messengerInfo,
        .settingCount = settingsCount,
        .pSettings = settings
    };

    instanceNext = &settingsInfo;
#endif

    VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = executableName,
        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
        .pEngineName = executableName,
        .engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
        .apiVersion = VK_API_VERSION_1_4
    };

    VkInstanceCreateInfo instanceInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = instanceNext,
        .flags = 0,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = layerCount,
        .ppEnabledLayerNames = layers,
        .enabledExtensionCount = extensionCount,
        .ppEnabledExtensionNames = extensions
    };

    debug("Instance layers (count = %d):", layerCount);
    for(uint32_t index = 0; index < layerCount; index++) {
        debug("\t%s", layers[index]);
    }

    debug("Instance extensions (count = %d):", extensionCount);
    for(uint32_t index = 0; index < extensionCount; index++) {
        debug("\t%s", extensions[index]);
    }

    vkCreateInstance(&instanceInfo, nullptr, &instance);
    debug("Instance created");
    free(extensions);

    PFN_vkGetInstanceProcAddr intermediateInstanceFunctionLoader = loadSystemFunction("vkGetInstanceProcAddr");
    instanceFunctionLoader = (PFN_vkGetInstanceProcAddr) intermediateInstanceFunctionLoader(instance, "vkGetInstanceProcAddr");
    debug("Instance function loader initialized");

#if DEBUG
    PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessenger = loadInstanceFunction("vkCreateDebugUtilsMessengerEXT");
    createDebugUtilsMessenger(instance, &messengerInfo, nullptr, &messenger);
    debug("Messenger created");
#endif
}

void destroyInstance() {
#if DEBUG
    PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessenger = loadInstanceFunction("vkDestroyDebugUtilsMessengerEXT");
    destroyDebugUtilsMessenger(instance, messenger, nullptr);
    debug("Messenger destroyed");
#endif

    vkDestroyInstance(instance, nullptr);
    debug("Instance destroyed");
}
