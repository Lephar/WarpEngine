#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vector>

namespace Engine::System {
	void initialize(const char* title, unsigned int& width, unsigned int& height);
	void* getLoader();
	std::vector<const char*> getExtensions();
	VkSurfaceKHR createSurface(VkInstance instance);
	void draw(void (*render)(void));
	void terminate();
}
