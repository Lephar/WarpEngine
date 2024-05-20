#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan_core.h>
#include <vector>

namespace System {
	class Window;

	Window *initialize(const char *title, int32_t width, int32_t height);
	Window *createWindow(const char *title, int32_t width, int32_t height);

	PFN_vkGetInstanceProcAddr getLoader(void);
	Window *getWindow(size_t index = 0);
	std::vector<const char *> getExtensions(size_t index = 0);

	void destroyWindow(Window *window);
	void destroyWindow(size_t index = 0);

	void quit(void);
}
