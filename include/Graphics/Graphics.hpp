#pragma once

#include "Zero.hpp"

namespace Graphics {
	void initialize();

	vk::CommandBuffer beginSingleTimeCommand();
	void endSingleTimeCommand(vk::CommandBuffer commandBuffer);

	void destroy();
}
