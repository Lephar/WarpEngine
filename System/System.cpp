module;

#include "System.hpp"

module Engine:System;

namespace Engine::System {
	const char* title;
	int width;
	int height;

	SDL_Window* window;

	void initialize(const char* title, unsigned int width, unsigned int height) {
		System::title = title;

		SDL_InitSubSystem(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO);

		window = SDL_CreateWindow(System::title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			width, height, SDL_WINDOW_VULKAN);

		SDL_Vulkan_GetDrawableSize(window, &System::width, &System::height);
	}

	const char* getTitle() {
		return title;
	}

	unsigned int getWidth() {
		return width;
	}

	unsigned int getHeight() {
		return height;
	}

	void* getLoader() {
		return SDL_Vulkan_GetVkGetInstanceProcAddr();
	}

	std::vector<const char*> getExtensions() {
		uint32_t extensionCount;
		SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);

		std::vector<const char*> extensions{ extensionCount };
		SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data());

		return extensions;
	}

	VkSurfaceKHR createSurface(VkInstance instance) {
		VkSurfaceKHR surface;

		SDL_Vulkan_CreateSurface(window, instance, &surface);

		return surface;
	}

	void draw() {
		while (true) {
			SDL_Event event;
			SDL_PollEvent(&event);

			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
				break;
		}
	}

	void terminate() {
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
}
