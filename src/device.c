#include "device.h"

#include "instance.h"

#include "queue.h"
#include "memory.h"
#include "helper.h"

VkPhysicalDevice physicalDevice;

VkPhysicalDeviceProperties physicalDeviceProperties;
VkPhysicalDeviceFeatures physicalDeviceFeatures;

VkDevice device;
PFN_vkGetDeviceProcAddr deviceLoader;

void *loadFunction(const char *name) {
    return deviceLoader(device, name);
}

void selectPhysicalDevice() {
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    VkPhysicalDevice *devices = malloc(deviceCount * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    uint32_t selectedIndex = 0;

    for(uint32_t deviceIndex = 1; deviceIndex < deviceCount; deviceIndex++) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(devices[deviceIndex], &properties);

        // TODO: Implement better device selection
        if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            selectedIndex = deviceIndex;
        }
    }

    physicalDevice = devices[selectedIndex];

    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);
    queueFamilyProperties = malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties);

    debug("Device selected: %s", physicalDeviceProperties.deviceName);

    free(devices);
}

void createDevice() {
    const char *extensionNames[] = {
        VK_KHR_MAINTENANCE_7_EXTENSION_NAME,
     // VK_KHR_MAINTENANCE_8_EXTENSION_NAME, // TODO: Enable when SDK updates
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
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
        .samplerAnisotropy = VK_TRUE,
    };

    VkPhysicalDeviceVulkan13Features deviceFeatures13 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = NULL,
        .dynamicRendering = VK_TRUE
    };

    VkPhysicalDeviceVulkan14Features deviceFeatures14 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        .pNext = &deviceFeatures13,
        .hostImageCopy = VK_TRUE
    };

    VkPhysicalDeviceMaintenance7FeaturesKHR maintenanceFeatures7 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_7_FEATURES_KHR,
        .pNext = &deviceFeatures14,
        .maintenance7 = VK_TRUE
    };
    /*
    VkPhysicalDeviceMaintenance8FeaturesKHR maintenanceFeatures8 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_8_FEATURES_KHR,
        .pNext = &maintenanceFeatures7,
        .maintenance78 = VK_TRUE
    };
    */
    VkPhysicalDeviceShaderObjectFeaturesEXT shaderObjectFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT,
        .pNext = &maintenanceFeatures7,
      //.pNext = &maintenanceFeatures8,
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

    PFN_vkGetDeviceProcAddr intermediateDeviceLoader = loadInstanceFunction("vkGetDeviceProcAddr");
    deviceLoader = (PFN_vkGetDeviceProcAddr) intermediateDeviceLoader(device, "vkGetDeviceProcAddr");
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
