#include "Graphics/DeviceInfo.hpp"

void DeviceInfo::initialize(vk::PhysicalDevice &physicalDevice, vk::SurfaceKHR &surface) {
    deviceProperties = physicalDevice.getProperties();
	deviceFeatures = physicalDevice.getFeatures();
	memoryProperties = physicalDevice.getMemoryProperties();
	queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
	surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);
	presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
	
	for(auto queueFamilyIndex = 0u; queueFamilyProperties.size(); queueFamilyIndex++)
		queueFamilySurfaceSupport.push_back(physicalDevice.getSurfaceSupportKHR(queueFamilyIndex, surface));
}