#pragma once

#include "Graphics.hpp"

class Device {
private:
	Renderer *owner;

    vk::PhysicalDevice physicalDevice;

	vk::Queue queue;

	vk::CommandPool commandPool;
	vk::CommandBuffer mainCommandBuffer;

	std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
	std::vector<vk::PhysicalDeviceMemoryProperties> memoryProperties;

	vk::Device device;

public:
	void initialize(Renderer *owner);
};