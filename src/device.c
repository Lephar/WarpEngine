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
    computeQueue .queueFamilyIndex = chooseQueueFamily(VK_QUEUE_COMPUTE_BIT );
    transferQueue.queueFamilyIndex = chooseQueueFamily(VK_QUEUE_TRANSFER_BIT);

    distinctQueueFamilyCount = 1;

    if(computeQueue.queueFamilyIndex == graphicsQueue.queueFamilyIndex) {
        computeQueue.queueIndex++;
    } else {
        computeQueue.queueInfoIndex++;
        distinctQueueFamilyCount++;
    }

    if(transferQueue.queueFamilyIndex == computeQueue.queueFamilyIndex) {
        transferQueue.queueInfoIndex = computeQueue.queueInfoIndex;
        transferQueue.queueIndex = computeQueue.queueIndex + 1;
    } else if(transferQueue.queueFamilyIndex == graphicsQueue.queueFamilyIndex) {
        transferQueue.queueInfoIndex = graphicsQueue.queueInfoIndex;
        transferQueue.queueIndex = graphicsQueue.queueIndex + 1;
    } else {
        transferQueue.queueInfoIndex = computeQueue.queueInfoIndex + 1;
        distinctQueueFamilyCount++;
    }
    
    VkDeviceQueueCreateInfo *queueInfos = malloc(distinctQueueFamilyCount * sizeof(VkDeviceQueueCreateInfo));
    
    for(uint32_t queueInfoIndex = 0; queueInfoIndex < distinctQueueFamilyCount; queueInfoIndex++) {
        queueInfos[queueInfoIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfos[queueInfoIndex].pNext = NULL;
        queueInfos[queueInfoIndex].flags = 0;
        queueInfos[queueInfoIndex].queueCount = 0,
        queueInfos[queueInfoIndex].pQueuePriorities = queuePriorities;
    }

    queueInfos[graphicsQueue.queueInfoIndex].queueFamilyIndex = graphicsQueue.queueFamilyIndex;
    queueInfos[computeQueue.queueInfoIndex ].queueFamilyIndex = computeQueue .queueFamilyIndex;
    queueInfos[transferQueue.queueInfoIndex].queueFamilyIndex = transferQueue.queueFamilyIndex;

    queueInfos[graphicsQueue.queueInfoIndex].queueCount++;
    queueInfos[computeQueue.queueInfoIndex ].queueCount++;
    queueInfos[transferQueue.queueInfoIndex].queueCount++;

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

    free(queueInfos);
}

void destroyDevice() {
    vkDestroyDevice(device, NULL);
}