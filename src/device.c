#include "device.h"
#include "queue.h"

extern VkInstance instance;

VkPhysicalDevice physicalDevice;

VkPhysicalDeviceProperties physicalDeviceProperties;
VkPhysicalDeviceFeatures physicalDeviceFeatures;

extern VkPhysicalDeviceMemoryProperties memoryProperties;

extern uint32_t queueFamilyCount;
extern VkQueueFamilyProperties *queueFamilyProperties;

VkDevice device;

extern Queue graphicsQueue;
extern Queue computeQueue;
extern Queue transferQueue;

uint32_t distinctQueueFamilyCount;

void selectPhysicalDevice() {
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
	VkPhysicalDevice *devices = malloc(deviceCount * sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    uint32_t selectedIndex = 0;

    for(uint32_t deviceIndex = 1; deviceIndex < deviceCount; deviceIndex++) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(devices[deviceIndex], &properties);
        
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

    free(devices);
}

void createDevice() {
    const char *extensionNames[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_SHADER_OBJECT_EXTENSION_NAME
    };

    uint32_t extensionCount = sizeof(extensionNames) / sizeof(const char *);

    VkPhysicalDeviceFeatures deviceFeatures = {
    };

    VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
        .pNext = NULL,
        .dynamicRendering = VK_TRUE
    };

    float queuePriorities[] = {
        1.0f,
        1.0f,
        1.0f
    };

    graphicsQueue.queueFamilyIndex = chooseQueueFamily(VK_QUEUE_GRAPHICS_BIT);
    computeQueue.queueFamilyIndex  = chooseQueueFamily(VK_QUEUE_COMPUTE_BIT);
    transferQueue.queueFamilyIndex = chooseQueueFamily(VK_QUEUE_TRANSFER_BIT);

    VkDeviceQueueCreateInfo queueInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = NULL,
        .flags = {},
        .queueFamilyIndex = UINT32_MAX,
        .queueCount = 1,
        .pQueuePriorities = queuePriorities
    };

    VkDeviceQueueCreateInfo queueInfos[] = {
        queueInfo,
        queueInfo,
        queueInfo
    };

    queueInfos[0].queueFamilyIndex = graphicsQueue.queueFamilyIndex;
    queueInfos[1].queueFamilyIndex = computeQueue.queueFamilyIndex;
    queueInfos[2].queueFamilyIndex = transferQueue.queueFamilyIndex;

    distinctQueueFamilyCount = 1;
    
    if(computeQueue.queueFamilyIndex == graphicsQueue.queueFamilyIndex) {
        computeQueue.queueIndex = graphicsQueue.queueIndex + 1;
        queueInfos[0].queueCount++;
    } else {
        queueInfo.queueFamilyIndex = computeQueue.queueFamilyIndex;
        distinctQueueFamilyCount++;
    }

    if(transferQueue.queueFamilyIndex == computeQueue.queueFamilyIndex) {
        transferQueue.queueIndex = computeQueue.queueIndex + 1;
        queueInfos[distinctQueueFamilyCount - 1].queueCount++;
    } else if(transferQueue.queueFamilyIndex == graphicsQueue.queueFamilyIndex) {
        transferQueue.queueIndex = graphicsQueue.queueIndex + 1;
        queueInfos[0].queueCount++;
    } else {
        queueInfo.queueFamilyIndex = transferQueue.queueFamilyIndex;
        distinctQueueFamilyCount++;
    }

    VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &dynamicRenderingFeatures,
        .flags = {},
        .queueCreateInfoCount = distinctQueueFamilyCount,
        .pQueueCreateInfos = queueInfos,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = extensionCount,
        .ppEnabledExtensionNames = extensionNames,
        .pEnabledFeatures = &deviceFeatures
    };

    vkCreateDevice(physicalDevice, &deviceInfo, NULL, &device);
}
