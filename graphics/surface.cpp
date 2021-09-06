#include "surface.hpp"

namespace zero::graphics {
	Surface::Surface(Instance &instance, system::Window &window) : instance(instance) {
		vk::XcbSurfaceCreateInfoKHR surfaceInfo{
			.connection = window.connection,
			.window = window.window
		};
		surface = instance.get().createXcbSurfaceKHR(surfaceInfo);
	}

	vk::SurfaceKHR &Surface::get(void) {
		return surface;
	}

	void Surface::destroy(void) {
		instance.get().destroySurfaceKHR(surface);
	}
}
