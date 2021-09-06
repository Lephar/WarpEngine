#include "surface.hpp"

namespace zero::graphics {
	Surface::Surface(Instance &instance, xcb_connection_t *connection, xcb_window_t window) : instance(instance) {
		vk::XcbSurfaceCreateInfoKHR surfaceInfo{
			.connection = connection,
			.window = window
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
