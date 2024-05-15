#pragma once

#include "Zero.hpp"

namespace System {
	class Window {
	private:
		const char *title;
		vk::Extent2D extent;
		SDL_Window* window;
		vk::SurfaceKHR surface;

	public:
		Window(const char *title, int32_t width, int32_t height);
		bool operator==(const Window &other);

		const char *getTitle();
		vk::Extent2D getExtent();
		uint32_t getWidth();
		uint32_t getHeight();
		std::vector<const char *> getExtensions();

		vk::SurfaceKHR createSurface();

		void draw(void (*render)(void));

		~Window();
	};
}
