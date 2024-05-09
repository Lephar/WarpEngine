#pragma once

#include "Graphics.hpp"

namespace Graphics {
	class DeviceInfo {
	public:
		vk::PhysicalDeviceProperties deviceProperties;
		vk::PhysicalDeviceFeatures deviceFeatures;
		vk::PhysicalDeviceMemoryProperties memoryProperties;
		std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
		vk::SurfaceCapabilitiesKHR surfaceCapabilities;
		std::vector<vk::SurfaceFormatKHR> surfaceFormats;
		std::vector<vk::PresentModeKHR> presentModes;
		std::vector<vk::Bool32> queueFamilySurfaceSupport;

		void populate(vk::PhysicalDevice &physicalDevice, vk::SurfaceKHR &surface);
	};
}
