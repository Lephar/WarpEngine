#include "device.h"

extern VkInstance instance;
VkPhysicalDevice physicalDevice;
VkDevice device;

void selectPhysicalDevice() {
    
    
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
	VkPhysicalDevice *devices = malloc(deviceCount * sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    for(uint32_t deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
        VkPhysicalDevice deviceCandidate = devices[deviceIndex];
        
        VkPhysicalDeviceProperties2 properties;
		VkPhysicalDeviceFeatures2 features;
    
        vkGetPhysicalDeviceProperties2(deviceCandidate, &properties);
        vkGetPhysicalDeviceFeatures2(deviceCandidate, &features);
    }
}