#include "System/System.hpp"
#include "System/Window.hpp"

namespace System {
	PFN_vkGetInstanceProcAddr loader;
	std::vector<Window *> windows;

	Window *initialize(const char *title, int32_t width, int32_t height) {
		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_Vulkan_LoadLibrary(nullptr);
		SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

		loader = reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());

		return createWindow(title, width, height);
	}

	Window *createWindow(const char *title, int32_t width, int32_t height) {
		Window *window = new Window(title, width, height);
		windows.push_back(window);

		return window;
	}

	PFN_vkGetInstanceProcAddr getLoader() {
		return loader;
	}

	Window *getWindow(size_t index) {
		return windows.at(index);
	}

	std::vector<const char *> getExtensions(size_t index) {
		return windows.at(index)->getExtensions();
	}

	void destroyWindow(Window *window) {
		auto position = std::find(windows.begin(), windows.end(), window);

		if(position != windows.end()) {
			windows.erase(position);
		}
	}

	void destroyWindow(size_t index) {
		if(index < windows.size()) {
			windows.erase(windows.begin() + index);
		}
	}

	void quit() {
		SDL_Vulkan_UnloadLibrary();
		SDL_Quit();
	}
}
