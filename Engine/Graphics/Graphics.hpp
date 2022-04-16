#pragma once

#define VULKAN_HPP_NO_SETTERS
#define VULKAN_HPP_NO_SMART_HANDLE
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_NODISCARD_WARNINGS
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

#include <limits>

#include <fstream>
#include <filesystem>

#ifndef NDEBUG
#include <iostream>
#endif //NDEBUG

namespace Engine::Graphics {
	struct PhysicalDevice {
		vk::PhysicalDevice physicalDevice;
		vk::PhysicalDeviceProperties properties;
		vk::PhysicalDeviceFeatures features;
		std::vector<vk::QueueFamilyProperties> queueFamilyPropertiesList;
		vk::PhysicalDeviceMemoryProperties memoryProperties;
		vk::SurfaceCapabilitiesKHR surfaceCapabilities;
		std::vector<vk::SurfaceFormatKHR> surfaceFormats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	struct Queue {
		unsigned int familyIndex;
		vk::Queue queue;
		vk::CommandPool commandPool;
		std::vector<vk::CommandBuffer> commandBuffers;
	};

	struct Details {
		vk::Format textureFormat;
		unsigned int mipLevels;
	};

	struct Memory {
		vk::DeviceSize size;
		vk::DeviceSize offset;
		vk::DeviceMemory memory;
		unsigned char* pointer;
	};

	struct Buffer {
		vk::DeviceSize size;
		vk::DeviceSize offset;
		vk::Buffer buffer;
		unsigned char* pointer;
	};

	struct Image {
		unsigned int width;
		unsigned int height;
		vk::Image image;
		vk::ImageView view;
	};

	struct Framebuffer {
		Image depthStencil;
		Image color;
		Image resolve;
	};

	struct Swapchain {
		unsigned int imageCount;
		unsigned int framebufferCount;

		vk::Extent2D extent;
		vk::Format depthStencilFormat;
		vk::Format colorFormat;

		vk::PresentModeKHR presentMode;
		vk::SurfaceFormatKHR surfaceFormat;
		vk::SampleCountFlagBits sampleCount;

		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> images;
		std::vector<Framebuffer> framebuffers;
	};

	vk::Instance createInstance(const char* engineTitle, void* loaderFunction, std::vector<const char*> instanceExtensions);
	void registerSurface(vk::SurfaceKHR surfaceHandle);
	void destroyCore();

	void allocateCommandBuffers(Queue& queue, unsigned int commandBufferCount);
	void createDevice();
	void freeCommandBuffers(Queue& queue);
	void destroyDevice();

	void createObjects();
	Image createImage(Memory& memory, unsigned int width, unsigned int height, unsigned int mips, vk::SampleCountFlagBits samples, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect);
	void destroyObjects();
	void destroyImage(Image& image);

	void createSwapchain(unsigned int width, unsigned int height);
	void destroySwapchain();
	void recreateSwapchain(unsigned int width, unsigned int height);
}
