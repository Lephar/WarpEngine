#include "physicalDevice.h"

#include "instance.h"
#include "surface.h"

#include "logger.h"

VkPhysicalDevice physicalDevice;

VkPhysicalDeviceProperties physicalDeviceProperties;
VkPhysicalDeviceFeatures physicalDeviceFeatures;

uint32_t queueFamilyCount;
VkQueueFamilyProperties *queueFamilyProperties;

VkPhysicalDeviceMemoryProperties memoryProperties;

void selectPhysicalDevice() {
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    VkPhysicalDevice *physicalDevices = malloc(physicalDeviceCount * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices);

    uint32_t discretePhysicalDeviceIndex   = UINT32_MAX;
    uint32_t integratedPhysicalDeviceIndex = UINT32_MAX;

    for(uint32_t physicalDeviceIndex = 0; physicalDeviceIndex < physicalDeviceCount; physicalDeviceIndex++) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevices[physicalDeviceIndex], &properties);

        if(getSurfaceSupport(physicalDevices[physicalDeviceIndex])) {
            if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                discretePhysicalDeviceIndex = physicalDeviceIndex;
                break;
            } else if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
                integratedPhysicalDeviceIndex = physicalDeviceIndex;
            }
        }
    }

    assert(discretePhysicalDeviceIndex != UINT32_MAX || integratedPhysicalDeviceIndex != UINT32_MAX);

    if(discretePhysicalDeviceIndex != UINT32_MAX) {
        physicalDevice = physicalDevices[discretePhysicalDeviceIndex];
    } else {
        physicalDevice = physicalDevices[integratedPhysicalDeviceIndex];
    }

    free(physicalDevices);
}

void generatePhysicalDeviceDetails() {
    VkPhysicalDeviceProperties2 physicalDeviceProperties2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext = nullptr,
        .properties = {}
    };

    vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);

    physicalDeviceProperties = physicalDeviceProperties2.properties;

    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = nullptr,
        .features = {}
    };

    vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);

    physicalDeviceFeatures = physicalDeviceFeatures2.features;

    VkPhysicalDeviceMemoryProperties2 memoryProperties2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2,
        .pNext = nullptr,
        .memoryProperties = {}
    };

    vkGetPhysicalDeviceMemoryProperties2(physicalDevice, &memoryProperties2);

    memoryProperties = memoryProperties2.memoryProperties;

    vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount, nullptr);

    VkQueueFamilyProperties2 *queueFamilyProperties2 = malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties2));

    for(uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++) {
        queueFamilyProperties2[queueFamilyIndex].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        queueFamilyProperties2[queueFamilyIndex].pNext = nullptr;
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
