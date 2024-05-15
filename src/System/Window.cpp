#include "System/Window.hpp"

namespace System {
	extern vk::Instance instance;

	Window::Window(const char *title, int32_t width, int32_t height) : title(title) {
		window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_VULKAN);
		SDL_Vulkan_GetDrawableSize(window, reinterpret_cast<int32_t *>(&extent.width), reinterpret_cast<int32_t *>(&extent.height));
	}

	bool Window::operator==(const Window &other) {
		return window == other.window;
	}

	std::vector<const char *> Window::getExtensions() {
		uint32_t extensionCount;
		SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);

		std::vector<const char *> extensions{ extensionCount };
		SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data());

		return extensions;
	}

	vk::SurfaceKHR Window::createSurface() {
		VkSurfaceKHR surfaceHandle;

		SDL_Vulkan_CreateSurface(window, instance, &surfaceHandle);

		surface = surfaceHandle;

		return surface;
	}

	void Window::draw(void (*render)(void)) {
		while (true) {
			SDL_Event event;
			SDL_PollEvent(&event);

			if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
				break;

			if(render)
				render();
		}
	}

	Window::~Window() {
		SDL_DestroyWindow(window);
		window = nullptr;

		extent.height = 0;
		extent.width = 0;
		title = "";
	}
}
