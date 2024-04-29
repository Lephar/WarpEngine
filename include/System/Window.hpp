#pragma once

#include "System.hpp"

class Window {
private:
	std::string title;
	SDL_Window* window;
	vk::Extent2D extent;

public:
    void initialize(std::string title, int width, int height);
    const char *getTitle();
    PFN_vkGetInstanceProcAddr getLoader();
    std::vector<const char *> getExtensions();
    vk::Extent2D getExtent();
    vk::SurfaceKHR createSurface(vk::Instance &instance);
    void draw(void (*render)(void));
};
