#ifndef ZERO_CLIENT_RENDER_HELPER_H
#define ZERO_CLIENT_RENDER_HELPER_H

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

#endif
