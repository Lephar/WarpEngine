#pragma once

#define VULKAN_HPP_NO_SETTERS
#define VULKAN_HPP_NO_SMART_HANDLE
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_NODISCARD_WARNINGS
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vulkan/vulkan.hpp>

struct Window {
	std::string title;

	SDL_Window* window;
	vk::Extent2D extent;
};

struct Core {
	PFN_vkGetInstanceProcAddr loader;

	vk::Instance instance;
#ifndef NDEBUG
	vk::DebugUtilsMessengerEXT messenger;
#endif // NDEBUG
	vk::SurfaceKHR surface;
};

struct Device {
	vk::PhysicalDevice physicalDevice;
	
	vk::PhysicalDeviceProperties properties;
	vk::PhysicalDeviceFeatures features;
	std::vector<vk::QueueFamilyProperties> queueFamilyPropertiesList;
	vk::PhysicalDeviceMemoryProperties memoryProperties;
	vk::SurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<vk::SurfaceFormatKHR> surfaceFormats;
	std::vector<vk::PresentModeKHR> presentModes;

	vk::Device device;
};

struct Swapchain {
	vk::PhysicalDevice physicalDevice;
	vk::Device device;

	vk::Queue transferQueue;
	vk::Queue graphicsQueue;

	vk::SwapchainKHR swapchain;
	std::vector<vk::Image> swapchainImages;
};

class Graphics {
private:
	Window window;
	Core core;
	Device device;
	Swapchain swapchain;

	void createWindow();
	void createInstance();
	void createSurface();

	vk::PhysicalDevice pickPhysicalDevice();
	void selectQueueFamilies(unsigned int& transferQueueFamilyIndex, unsigned int& graphicsQueueFamilyIndex);
	void createDevice();

	void createSwapchain();

public:
	Graphics(const char* title, unsigned int width, unsigned int height);

	void draw(void (*render)(void));

	~Graphics();
};
