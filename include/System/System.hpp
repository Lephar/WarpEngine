#pragma once

#include "Zero.hpp"

namespace System {
	struct Window;

	void initialize();

	PFN_vkGetInstanceProcAddr getLoader();

	Window createWindow(std::string title, int width, int height);
	void destroyWindow(Window window);

	void quit();
}
