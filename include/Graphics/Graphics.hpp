#pragma once

#define VULKAN_HPP_NO_SETTERS
#define VULKAN_HPP_NO_SMART_HANDLE
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_NODISCARD_WARNINGS
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>

namespace Graphics {
	void initialize();

	vk::Instance getInstance();
	vk::Device getDevice();

	vk::CommandBuffer beginSingleTimeCommand();
	void endSingleTimeCommand(vk::CommandBuffer commandBuffer);

	void destroy();
}