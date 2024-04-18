#pragma once

#include "System/Window.hpp"

#include "Device.hpp"
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

	Device device;
	vk::PhysicalDevice physicalDevice;

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
