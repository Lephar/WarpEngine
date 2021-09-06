#include "graphics.hpp"

namespace zero::graphics {
	Graphics::Graphics(xcb_connection_t *connection) : connection(connection),
													   instance("Zero", "ZeroEngine", VK_MAKE_API_VERSION(0, 0, 1, 0)) {

	}

	Renderer& Graphics::createRenderer(xcb_window_t window) {
		renderers.emplace_back(instance, connection, window);
		return renderers.back();
	}
}