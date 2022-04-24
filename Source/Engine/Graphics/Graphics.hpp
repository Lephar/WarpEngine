#pragma once

#define VULKAN_HPP_NO_SETTERS
#define VULKAN_HPP_NO_SMART_HANDLE
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_NODISCARD_WARNINGS
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>
#include <glm/glm.hpp>

#include <limits>
#include <fstream>
#include <filesystem>

#ifndef NDEBUG
#include <iostream>
#endif //NDEBUG

namespace Engine::Graphics {
	struct Vertex {
		glm::vec3 position;
		glm::vec2 texture;
	};

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
		unsigned familyIndex;
		vk::Queue queue;
		vk::CommandPool commandPool;
	};

	struct Details {
		vk::Format textureFormat;
		unsigned mipLevels;
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
		unsigned width;
		unsigned height;
		vk::Image image;
		vk::ImageView view;
	};

	struct Framebuffer {
		Image depthStencil;
		Image color;
		Image resolve;

		vk::Semaphore renderingSemaphore;
		vk::Semaphore acquisitionSemaphore;
		vk::Semaphore blittingSemaphore;

		vk::Fence renderingFence;
		vk::Fence blittingFence;

		vk::CommandBuffer renderingCommandBuffer;
		std::vector<vk::CommandBuffer> blittingCommandBuffers;
	};

	struct Swapchain {
		unsigned imageCount;
		unsigned framebufferCount;

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

	struct Pipeline {
		unsigned mipLevels;
		float maxAnisotropy;

		vk::ShaderModule vertexShader;
		vk::ShaderModule fragmentShader;

		vk::DescriptorSetLayout descriptorSetLayout;
		vk::PipelineLayout pipelineLayout;

		vk::Sampler sampler;

		vk::Pipeline pipeline;
	};

	vk::Instance createInstance(const char* engineTitle, void* loaderFunction, std::vector<const char*> instanceExtensions);
	void registerSurface(vk::SurfaceKHR surfaceHandle);
	void destroyCore();

	std::vector<vk::CommandBuffer> allocateCommandBuffers(Queue& queue, unsigned commandBufferCount);
	void createDevice();
	void freeCommandBuffers(Queue& queue, std::vector<vk::CommandBuffer>& commandBuffers);
	void destroyDevice();

	void createObjects();
	Image createImage(Memory& memory, unsigned width, unsigned height, unsigned mips, vk::SampleCountFlagBits samples, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect);
	void destroyObjects();
	void destroyImage(Image& image);

	void createSwapchain(unsigned width, unsigned height);
	void destroySwapchain();
	void recreateSwapchain(unsigned width, unsigned height);

	void createPipeline();
	void destroyPipeline();
}
