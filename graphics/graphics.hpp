#ifndef ZERO_CLIENT_GRAPHICS_HPP
#define ZERO_CLIENT_GRAPHICS_HPP

#include "header.hpp"
#include "instance.hpp"
#include "renderer.hpp"

namespace zero::graphics {
	class Graphics {
	private:
		xcb_connection_t* connection;
		Instance instance;
		std::vector<Renderer> renderers;
	public:
		Graphics(xcb_connection_t* connection);
		Renderer& createRenderer(xcb_window_t window);
	};
}

#endif //ZERO_CLIENT_GRAPHICS_HPP
