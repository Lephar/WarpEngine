#include "device.h"

#include "instance.h"
#include "surface.h"
#include "physicalDevice.h"
#include "queue.h"

#include "logger.h"

VkDevice device;
PFN_vkGetDeviceProcAddr deviceFunctionLoader;

void *loadDeviceFunction(const char *name) {
    void  *deviceFunction = deviceFunctionLoader(device, name);
    assert(deviceFunction);
    return deviceFunction;
}

void createDevice() {
    const char *baseExtensions[] = {
        VK_KHR_MAINTENANCE_7_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_8_EXTENSION_NAME,
        VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
    };

    uint32_t baseExtensionCount = sizeof(baseExtensions) / sizeof(const char *);

    uint32_t extensionCount = baseExtensionCount + surfaceDeviceExtensionCount;
    const char **extensions = malloc(extensionCount * sizeof(const char *));

    memcpy(extensions, baseExtensions, baseExtensionCount * sizeof(const char *));
    memcpy(extensions + baseExtensionCount, surfaceDeviceExtensions, surfaceDeviceExtensionCount * sizeof(const char *));

    debug("Device extensions (count = %d):", extensionCount);
    for(uint32_t index = 0; index < extensionCount; index++) {
        debug("\t%s", extensions[index]);
    }

    float *queuePriorities = malloc(queueCount * sizeof(float));

    for(uint32_t index = 0; index < queueCount; index++) {
        queuePriorities[index] = 1.0f;
    }

    queueInfos = malloc(distinctQueueFamilyCount * sizeof(VkDeviceQueueCreateInfo));

    for(uint32_t queueInfoIndex = 0; queueInfoIndex < distinctQueueFamilyCount; queueInfoIndex++) {
        queueInfos[queueInfoIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfos[queueInfoIndex].pNext = nullptr;
        queueInfos[queueInfoIndex].flags = 0;
        queueInfos[queueInfoIndex].queueCount = 0,
        queueInfos[queueInfoIndex].pQueuePriorities = queuePriorities;
    }

    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        VkDeviceQueueCreateInfo *queueInfo = &queueInfos[queueReferences[queueIndex]->queueInfoIndex];
        queueInfo->queueFamilyIndex = queueReferences[queueIndex]->queueFamilyIndex;

        if(queueInfo->queueCount < queueFamilyProperties[queueInfo->queueFamilyIndex].queueCount) {
            queueInfo->queueCount++;
        }
    }

    VkPhysicalDeviceShaderObjectFeaturesEXT shaderObjectFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT,
        .pNext = surfaceDeviceFeatures,
        .shaderObject = VK_TRUE,
    };

    VkPhysicalDeviceMaintenance8FeaturesKHR maintenance8Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_8_FEATURES_KHR,
        .pNext = &shaderObjectFeatures,
        .maintenance8 = VK_TRUE,
    };

    VkPhysicalDeviceMaintenance7FeaturesKHR maintenance7Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_7_FEATURES_KHR,
        .pNext = &maintenance8Features,
        .maintenance7 = VK_TRUE,
    };

    VkPhysicalDeviceVulkan14Features version14Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        .pNext = &maintenance7Features,
        .hostImageCopy = VK_TRUE,
    };

    VkPhysicalDeviceVulkan13Features version13Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &version14Features,
        .dynamicRendering = VK_TRUE,
    };

    VkPhysicalDeviceVulkan12Features version12Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &version13Features,
    };

    VkPhysicalDeviceVulkan11Features version11Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = &version12Features,
    };

    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &version11Features,
        .features = {
            .fillModeNonSolid = VK_TRUE,
            .wideLines = VK_TRUE,
            .largePoints = VK_TRUE,
            .samplerAnisotropy = VK_TRUE,
        },
    };

    VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &physicalDeviceFeatures2,
        .flags = 0,
        .queueCreateInfoCount = distinctQueueFamilyCount,
        .pQueueCreateInfos = queueInfos,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = extensionCount,
        .ppEnabledExtensionNames = extensions,
        .pEnabledFeatures = nullptr,
    };

    vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device);
    debug("Device created");
    free(queuePriorities);
    free(extensions);

    PFN_vkGetDeviceProcAddr instanceDeviceFunctionLoader = loadInstanceFunction("vkGetDeviceProcAddr");
    deviceFunctionLoader = (PFN_vkGetDeviceProcAddr) instanceDeviceFunctionLoader(device, "vkGetDeviceProcAddr");
    debug("Device function loader initialized");
}

void destroyDevice() {
    vkDeviceWaitIdle(device);
    vkDestroyDevice(device, nullptr);

    free(queueFamilyProperties);
    free(queueInfos);

    debug("Device destroyed");
}
