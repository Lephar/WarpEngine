module;

#include "Engine-Graphics.hpp"

module Engine:Graphics;

import :System;

namespace Engine::Graphics {
	extern vk::Device device;

	extern Queue graphicsQueue;

	Swapchain::Swapchain(unsigned int imageCount, unsigned int activeImageCount) {
	}

	void Swapchain::destroy() {

	}
}