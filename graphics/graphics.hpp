#ifndef ZERO_CLIENT_GRAPHICS_HPP
#define ZERO_CLIENT_GRAPHICS_HPP

#include "header.hpp"
#include "instance.hpp"
#include "renderer.hpp"

namespace zero::graphics {
	class Graphics {
	private:
		Instance instance;
		std::vector<Renderer> renderers;
	public:
		Graphics(void);
		Renderer &createRenderer(system::Window window);
	};
}

#endif //ZERO_CLIENT_GRAPHICS_HPP
