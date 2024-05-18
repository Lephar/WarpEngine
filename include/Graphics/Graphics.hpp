#pragma once

#include "Zero.hpp"

namespace Graphics {
	void initialize(const char *title, PFN_vkGetInstanceProcAddr loader, std::vector<const char *> extensions);
	vk::Instance getInstance(void);
	void registerSurface(vk::SurfaceKHR surface);

	vk::CommandBuffer beginSingleTimeCommand(void);
	void endSingleTimeCommand(vk::CommandBuffer commandBuffer);

	void destroy();
}
