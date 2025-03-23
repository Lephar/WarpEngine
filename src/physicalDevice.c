#include "physicalDevice.h"

#include "instance.h"

#include "logger.h"

VkPhysicalDevice physicalDevice;

VkPhysicalDeviceProperties physicalDeviceProperties;
VkPhysicalDeviceFeatures physicalDeviceFeatures;

uint32_t queueFamilyCount;
VkQueueFamilyProperties *queueFamilyProperties;

VkPhysicalDeviceMemoryProperties memoryProperties;

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

    free(devices);

    VkPhysicalDeviceProperties2 physicalDeviceProperties2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext = NULL,
        .properties = {}
    };

    vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);

    physicalDeviceProperties = physicalDeviceProperties2.properties;

    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = NULL,
        .features = {}
    };

    vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);

    physicalDeviceFeatures = physicalDeviceFeatures2.features;

    VkPhysicalDeviceMemoryProperties2 memoryProperties2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2,
        .pNext = NULL,
        .memoryProperties = {}
    };

    vkGetPhysicalDeviceMemoryProperties2(physicalDevice, &memoryProperties2);

    memoryProperties = memoryProperties2.memoryProperties;

    vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount, NULL);

    VkQueueFamilyProperties2 *queueFamilyProperties2 = malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties2));

    for(uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++) {
        queueFamilyProperties2[queueFamilyIndex].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        queueFamilyProperties2[queueFamilyIndex].pNext = NULL;
        memset(&queueFamilyProperties2[queueFamilyIndex].queueFamilyProperties, 0, sizeof(VkQueueFamilyProperties));
    }

    vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount, queueFamilyProperties2);

    queueFamilyProperties = malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));

    for(uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++) {
        queueFamilyProperties[queueFamilyIndex] = queueFamilyProperties2[queueFamilyIndex].queueFamilyProperties;
    }

    debug("Physical device selected: %s", physicalDeviceProperties.deviceName);

    free(queueFamilyProperties2);
}
