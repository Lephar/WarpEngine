#include "renderer.hpp"

namespace zero::graphics {
	Renderer::Renderer(Instance &instance, xcb_connection_t *connection, xcb_window_t window) : instance(instance),
																								surface(instance,
																										connection,
																										window),
																								internals(instance, surface) {

	}
}