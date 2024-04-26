#pragma once

#include "System/Window.hpp"

#include "DeviceInfo.hpp"
#include "Memory.hpp"
#include "Image.hpp"
#include "Buffer.hpp"
#include "Framebuffer.hpp"

#ifndef NDEBUG
	VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
#endif // NDEBUG

class Renderer {
private:
	Window *window;

	PFN_vkGetInstanceProcAddr loader;

	vk::Instance instance;
#ifndef NDEBUG
	vk::DebugUtilsMessengerEXT messenger;
#endif // NDEBUG
	vk::SurfaceKHR surface;

	DeviceInfo deviceInfo;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
	vk::Queue queue;
	vk::CommandPool commandPool;

	Memory hostMemory;
	Memory deviceMemory;
	Memory imageMemory;

	Buffer uniformBuffer;
	Buffer stagingBuffer;
	Buffer elementBuffer;

	uint32_t swapchainSize;
	vk::SwapchainKHR swapchain;
	std::vector<vk::Image> swapchainImages;

	vk::SampleCountFlagBits sampleCount;
	vk::Format depthStencilFormat;
	vk::Format colorFormat;
	uint32_t mipCount;
	std::vector<Framebuffer> framebuffers;

	void createInstance();
	void createSurface();

	vk::PhysicalDevice pickPhysicalDevice();
	uint32_t selectQueueFamily();
	void createDevice();

	vk::SurfaceFormatKHR selectSurfaceFormat();
	vk::PresentModeKHR selectPresentMode();

	void createSwapchain();
	void createFramebuffers();

	void createBuffers();
public:
	void initialize(Window *window);

	vk::Instance getInstance();
	vk::Device getDevice();

	vk::CommandBuffer beginSingleTimeCommand();
	void endSingleTimeCommand(vk::CommandBuffer commandBuffer);

	void draw(void (*render)(void) = nullptr);

	void destroy();
};
