#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Graphics {
	class Device {
	private:
		vk::raii::PhysicalDevice physicalDevice;

		vk::PhysicalDeviceProperties properties;
		vk::PhysicalDeviceFeatures features;
		vk::PhysicalDeviceMemoryProperties memoryProperties;
		std::vector<vk::QueueFamilyProperties> queueFamilyProperties;

		vk::raii::Device *device;
	public:
		Device(vk::raii::PhysicalDevice physicalDevice);

		~Device(void);
	};
}
