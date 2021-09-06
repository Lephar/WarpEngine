#ifndef ZERO_CLIENT_GRAPHICS_SURFACE_HPP
#define ZERO_CLIENT_GRAPHICS_SURFACE_HPP

#include "header.hpp"
#include "instance.hpp"

namespace zero::graphics {
	class Surface {
	private:
		Instance &instance;
		vk::SurfaceKHR surface;
	public:
		Surface(Instance &instance, xcb_connection_t *connection, xcb_window_t window);
		vk::SurfaceKHR &get(void);
		void destroy(void);
	};
}

#endif //ZERO_CLIENT_GRAPHICS_SURFACE_HPP
