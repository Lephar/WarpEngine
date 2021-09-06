#ifndef ZERO_CLIENT_GRAPHICS_INTERNALS_HPP
#define ZERO_CLIENT_GRAPHICS_INTERNALS_HPP

#include "header.hpp"
#include "instance.hpp"
#include "surface.hpp"

namespace zero::graphics {
	class Internals {
	public:
		Surface &surface;
		vk::PhysicalDevice physicalDevice;
		uint32_t transferQueueFamily;
		uint32_t graphicsQueueFamily;
		Internals(Instance &instance, Surface &surface);
	};
}

#endif //ZERO_CLIENT_GRAPHICS_INTERNALS_HPP
