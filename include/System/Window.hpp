#pragma once

#include "Zero.hpp"

namespace System {
	class Window {
	private:
		vk::Extent2D extent;
		SDL_Window *window;

	public:
		Window(const char *title, int32_t width, int32_t height);

		vk::Extent2D getExtent(void);
		std::vector<const char *> getExtensions();

		vk::SurfaceKHR createSurface(vk::Instance instance);

		void draw(void (*render)(void));

		~Window(void);
	};
}
