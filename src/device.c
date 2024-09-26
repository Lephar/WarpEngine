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

    Queue *queues[] = {
        &graphicsQueue,
        &computeQueue,
        &transferQueue
    };

    uint32_t queueCount = sizeof(queues) / sizeof(Queue *);

    distinctQueueFamilyCount = 1;

    for(uint32_t queueIndex = 1; queueIndex < queueCount; queueIndex++) {
        VkBool32 queueDistinct = VK_TRUE;
        uint32_t comparisonIndex = queueIndex - 1;

        while(1) {
            if(queues[queueIndex]->queueFamilyIndex == queues[comparisonIndex]->queueFamilyIndex) {
                queues[queueIndex]->queueInfoIndex = queues[comparisonIndex]->queueInfoIndex;
                queues[queueIndex]->queueIndex = queues[comparisonIndex]->queueIndex + 1;

                queueDistinct = VK_FALSE;
                break;
            }

            if(!comparisonIndex--){
                break;
            }
        }

        if(queueDistinct) {
            queues[queueIndex]->queueInfoIndex = distinctQueueFamilyCount++;
        }
    }

    VkDeviceQueueCreateInfo *queueInfos = malloc(distinctQueueFamilyCount * sizeof(VkDeviceQueueCreateInfo));
    
    for(uint32_t queueInfoIndex = 0; queueInfoIndex < distinctQueueFamilyCount; queueInfoIndex++) {
        queueInfos[queueInfoIndex].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfos[queueInfoIndex].pNext = NULL;
        queueInfos[queueInfoIndex].flags = 0;
        queueInfos[queueInfoIndex].queueCount = 0,
        queueInfos[queueInfoIndex].pQueuePriorities = queuePriorities;
    }

    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        queueInfos[queues[queueIndex]->queueInfoIndex].queueFamilyIndex = queues[queueIndex]->queueFamilyIndex;
        queueInfos[queues[queueIndex]->queueInfoIndex].queueCount++;
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

    free(queueInfos);
}

void destroyDevice() {
    vkDestroyDevice(device, NULL);
}