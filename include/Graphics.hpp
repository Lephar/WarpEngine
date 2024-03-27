#pragma once

#define VULKAN_HPP_NO_SETTERS
#define VULKAN_HPP_NO_SMART_HANDLE
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_NODISCARD_WARNINGS
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.hpp>

#ifndef NDEBUG
	VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
#endif // NDEBUG

class Graphics {
public:
	struct Memory {
		vk::DeviceMemory memory;
		vk::DeviceSize size;
		vk::DeviceSize offset;
	};

	struct Buffer {
		vk::DeviceMemory &memory;
		vk::Buffer buffer;
		vk::DeviceSize size;
		vk::DeviceSize offset;
	};

	struct Image {
		vk::Image image;
		vk::ImageView view;
	};

	struct Framebuffer {
		Image depthStencil;
		Image color;
		Image resolve;

		vk::CommandBuffer draw;
		vk::CommandBuffer drawBuffer;
	};

private:
	std::string title;

	SDL_Window* window;
	vk::Extent2D extent;

	PFN_vkGetInstanceProcAddr loader;

	vk::Instance instance;
#ifndef NDEBUG
	vk::DebugUtilsMessengerEXT messenger;
#endif // NDEBUG
	vk::SurfaceKHR surface;
	
	vk::PhysicalDevice physicalDevice;
	vk::Device device;

	vk::Queue queue;

	vk::CommandPool commandPool;
	vk::CommandBuffer mainCommandBuffer;

	vk::PhysicalDeviceMemoryProperties memoryProperties;

	Memory hostMemory;
	Memory deviceMemory;
	Memory imageMemory;

	uint32_t swapchainSize;
	vk::SwapchainKHR swapchain;
	std::vector<vk::Image> swapchainImages;

	uint32_t framebufferCount;
	vk::SampleCountFlagBits sampleCount;
	vk::Format depthStencilFormat;
	vk::Format colorFormat;
	uint32_t mipCount;

	std::vector<Framebuffer> framebuffers;

	void createWindow();
	void createInstance();
	void createSurface();

	vk::PhysicalDevice pickPhysicalDevice();
	uint32_t selectQueueFamily();
	void createDevice();

	vk::SurfaceFormatKHR selectSurfaceFormat();
	vk::PresentModeKHR selectPresentMode();

	vk::CommandBuffer beginSingleTimeCommand();
	void endSingleTimeCommand(vk::CommandBuffer commandBuffer);
	uint32_t chooseMemoryType(uint32_t filter, vk::MemoryPropertyFlags properties);
	vk::DeviceMemory allocateMemory(vk::DeviceSize size, uint32_t index);
	vk::Buffer createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage);
	void copyBuffer(vk::Buffer source, vk::Buffer destination, vk::DeviceSize size);
	vk::Image createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags usage, vk::SampleCountFlagBits samples, uint32_t mips);
	vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspects, uint32_t mips);
	void bindImageMemory(vk::Image image);
	void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
	void transitionImageLayout(vk::Image image, vk::ImageAspectFlags aspects, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

	void createSwapchain();
	void createFramebuffers();

	void createBuffers();
public:
	Graphics(std::string title, uint32_t width, uint32_t height);

	void draw(void (*render)(void) = nullptr);

	~Graphics();
};
