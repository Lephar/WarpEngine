#include "renderSystemDevice.h"

//TODO: Implement a better device selection
VkPhysicalDevice pickPhysicalDevice(VkInstance instance) {
    uint32_t physicalDeviceCount = 0u;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
    VkPhysicalDevice physicalDevices[physicalDeviceCount];
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices);

    for (uint32_t index = 0u; index <= physicalDeviceCount; index++) {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevices[index], &physicalDeviceProperties);

        if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            return physicalDevices[index];
    }

    return physicalDevices[0];
}

//TODO: Add exclusive queue family support
uint32_t selectQueueFamily(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice) {
    uint32_t queueFamilyPropertyCount = 0u;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, NULL);
    VkQueueFamilyProperties queueFamilyProperties[queueFamilyPropertyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties);

    for (uint32_t index = 0u; index < queueFamilyPropertyCount; index++) {
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, surface, &presentSupport);
        VkQueueFlags graphicsSupport = queueFamilyProperties[index].queueFlags & VK_QUEUE_GRAPHICS_BIT;

        if (presentSupport && graphicsSupport)
            return index;
    }

    return UINT32_MAX;
}

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) {
    float queuePriority = 1.0f;
    VkPhysicalDeviceFeatures deviceFeatures = {};
    uint32_t extensionCount = 1u;
    const char *extensionNames[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkDeviceQueueCreateInfo queueInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
    };

    VkDeviceCreateInfo deviceInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queueInfo,
            .pEnabledFeatures = &deviceFeatures,
            .enabledExtensionCount = extensionCount,
            .ppEnabledExtensionNames = extensionNames
    };

    VkDevice logicalDevice;
    vkCreateDevice(physicalDevice, &deviceInfo, NULL, &logicalDevice);
    return logicalDevice;
}

VkQueue getDeviceQueue(VkDevice logicalDevice, uint32_t queueFamilyIndex) {
    VkQueue queue;
    vkGetDeviceQueue(logicalDevice, queueFamilyIndex, 0, &queue);
    return queue;
}

VkCommandPool createCommandPool(VkDevice logicalDevice, uint32_t queueFamilyIndex) {
    VkCommandPoolCreateInfo poolInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = queueFamilyIndex
    };

    VkCommandPool commandPool;
    vkCreateCommandPool(logicalDevice, &poolInfo, NULL, &commandPool);
    return commandPool;
}

zrDevice createDevice(zrCore core) {
    zrDevice device;
    device.physicalDevice = pickPhysicalDevice(core.instance);
    uint32_t queueFamilyIndex = selectQueueFamily(core.surface, device.physicalDevice);

    device.device = createLogicalDevice(device.physicalDevice, queueFamilyIndex);
    device.queue = getDeviceQueue(device.device, queueFamilyIndex);
    device.commandPool = createCommandPool(device.device, queueFamilyIndex);

    return device;
}

void destroyDevice(zrDevice device) {
    vkDestroyCommandPool(device.device, device.commandPool, NULL);
    vkDestroyDevice(device.device, NULL);
}
