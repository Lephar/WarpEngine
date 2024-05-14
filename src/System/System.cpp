#include "System/System.hpp"
#include "System/Window.hpp"

namespace System {
	PFN_vkGetInstanceProcAddr loader;
	std::vector<const char *> extensions;

	void initialize() {
		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_Vulkan_LoadLibrary(nullptr);
		SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

		loader = reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());

		SDL_Window *window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN);

		uint32_t extensionCount;
        SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);

        extensions.resize(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data());

		SDL_DestroyWindow(window);
	}

	PFN_vkGetInstanceProcAddr getLoader() {
		return loader;
	}

	std::vector<const char *> getExtensions() {
        return extensions;
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
