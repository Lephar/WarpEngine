#pragma once

#include "System.hpp"

struct Window {
	std::string title;
	SDL_Window* window;
	vk::Extent2D extent;

    
};
