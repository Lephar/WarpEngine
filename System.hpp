#pragma once

#define VULKAN_HPP_NO_SETTERS
#define VULKAN_HPP_NO_SMART_HANDLE
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_NODISCARD_WARNINGS
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vector>

class System
{
private:
	const char* title;
	int width;
	int height;
	SDL_Window* window;
public:
	System(const char *title, int width, int height);
	std::vector<const char *> getInstanceExtensions(void);
	PFN_vkGetInstanceProcAddr getInstanceProcAddr(void);
	vk::SurfaceKHR createSurface(vk::Instance instance);
	void mainLoop(void);
	~System(void);
};
