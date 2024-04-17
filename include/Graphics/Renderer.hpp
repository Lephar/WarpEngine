#pragma once

#include "Graphics.hpp"

#include "Memory.hpp"
#include "Image.hpp"
#include "Buffer.hpp"
#include "Framebuffer.hpp"

#ifndef NDEBUG
	VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
#endif // NDEBUG

class Renderer {
	friend class Memory;
	friend class Image;
	friend class Buffer;

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

	Buffer uniformBuffer;
	Buffer stagingBuffer;
	Buffer elementBuffer;

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

	void createSwapchain();
	void createFramebuffers();

	void createBuffers();
public:
	Renderer(std::string title, uint32_t width, uint32_t height);

	void draw(void (*render)(void) = nullptr);

	~Renderer();
};
