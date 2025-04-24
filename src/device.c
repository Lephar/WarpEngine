#include "device.h"

#include "instance.h"
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
    const char *extensionNames[] = {
        VK_KHR_MAINTENANCE_7_EXTENSION_NAME,
         // VK_KHR_MAINTENANCE_8_EXTENSION_NAME, // TODO: Enable when driver updates
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME,
        VK_EXT_SHADER_OBJECT_EXTENSION_NAME
    };

    uint32_t extensionCount = sizeof(extensionNames) / sizeof(const char *);

    debug("Device extensions (count = %d):", extensionCount);
    for(uint32_t index = 0; index < extensionCount; index++) {
        debug("\t%s", extensionNames[index]);
    }

    float *queuePriorities = malloc(queueCount * sizeof(float));

    for(uint32_t index = 0; index < queueCount; index++) {
        queuePriorities[index] = 1.0f;
    }

    queueInfos = malloc(distinctQueueFamilyCount * sizeof(VkDeviceQueueCreateInfo));

    for(uint32_t queueInfoIndex = 0; queueInfoIndex < distinctQueueFamilyCount; queueInfoIndex++) {
        queueInfos[queueInfoIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfos[queueInfoIndex].pNext = NULL;
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

    VkPhysicalDeviceFeatures deviceFeatures = {
        .fillModeNonSolid = VK_TRUE,
        .wideLines = VK_TRUE,
        .largePoints = VK_TRUE,
        .samplerAnisotropy = VK_TRUE
    };

    VkPhysicalDeviceVulkan11Features version11Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = NULL
    };

    VkPhysicalDeviceVulkan12Features version12Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &version11Features
    };

    VkPhysicalDeviceVulkan13Features version13Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &version12Features,
        .dynamicRendering = VK_TRUE
    };

    VkPhysicalDeviceVulkan14Features version14Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        .pNext = &version13Features,
        .hostImageCopy = VK_TRUE
    };

    VkPhysicalDeviceMaintenance7FeaturesKHR maintenance7Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_7_FEATURES_KHR,
        .pNext = &version14Features,
        .maintenance7 = VK_TRUE
    };
    /*
    VkPhysicalDeviceMaintenance8FeaturesKHR maintenance8Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_8_FEATURES_KHR,
        .pNext = &maintenance7Features,
        .maintenance8 = VK_TRUE
    };
    */
    VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT swapchainMaintenance1Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_EXT,
        .pNext = &maintenance7Features,
      //.pNext = &maintenance8Features,
        .swapchainMaintenance1 = VK_TRUE
    };

    VkPhysicalDeviceShaderObjectFeaturesEXT shaderObjectFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT,
        .pNext = &swapchainMaintenance1Features,
        .shaderObject = VK_TRUE
    };

    VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &shaderObjectFeatures,
        .flags = 0,
        .queueCreateInfoCount = distinctQueueFamilyCount,
        .pQueueCreateInfos = queueInfos,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = extensionCount,
        .ppEnabledExtensionNames = extensionNames,
        .pEnabledFeatures = &deviceFeatures
    };

    vkCreateDevice(physicalDevice, &deviceInfo, NULL, &device);
    debug("Device created");

    PFN_vkGetDeviceProcAddr intermediateDeviceFunctionLoader = loadInstanceFunction("vkGetDeviceProcAddr");
    deviceFunctionLoader = (PFN_vkGetDeviceProcAddr) intermediateDeviceFunctionLoader(device, "vkGetDeviceProcAddr");
    debug("Device function loader initialized");

    free(queuePriorities);
}

void destroyDevice() {
    vkDeviceWaitIdle(device);
    vkDestroyDevice(device, NULL);

    free(queueFamilyProperties);
    free(queueInfos);

    debug("Device destroyed");
}
