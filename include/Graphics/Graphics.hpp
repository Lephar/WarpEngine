#pragma once

#include "System.hpp"

namespace Graphics {
	void initialize();

	vk::CommandBuffer beginSingleTimeCommand();
	void endSingleTimeCommand(vk::CommandBuffer commandBuffer);

	void destroy();
}