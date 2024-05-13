#pragma once

#include "Zero.hpp"

namespace System {
	struct Window {
		std::string title;
		SDL_Window* window;
		vk::Extent2D extent;

		std::vector<const char *> getExtensions();
		vk::SurfaceKHR createSurface(vk::Instance &instance);

		void draw(void (*render)(void));
	};
}
