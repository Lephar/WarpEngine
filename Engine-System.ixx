module;

#include "Engine-System.hpp"

export module Engine:System;

namespace Engine::System {
	void initialize(const char* title, unsigned int width, unsigned int height);
	const char* getTitle();
	unsigned int getWidth();
	unsigned int getHeight();
	void* getLoader();
	std::vector<const char*> getExtensions();
	VkSurfaceKHR createSurface(VkInstance instance);
	void draw();
	void terminate();
}
