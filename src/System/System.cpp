#include "System/System.hpp"
#include "System/Window.hpp"
#include <SDL_video.h>

namespace System {
	void initialize() {
		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_Vulkan_LoadLibrary(nullptr);
		SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
	}

	PFN_vkGetInstanceProcAddr getLoader() {
		return reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());
	}

	Window createWindow(std::string title, int width, int height) {
		Window window;

		window.title = title;
		window.window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_VULKAN);
		SDL_Vulkan_GetDrawableSize(window.window, reinterpret_cast<int32_t *>(&window.extent.width), reinterpret_cast<int32_t *>(&window.extent.height));

		return window;
	}

	void destroyWindow(Window window) {
		SDL_DestroyWindow(window.window);
	}

	void quit() {
		SDL_Vulkan_UnloadLibrary();
		SDL_Quit();
	}
}
