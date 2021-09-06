#include "graphics.hpp"

namespace zero::graphics {
	Graphics::Graphics(void) : instance("Zero", "ZeroEngine", VK_MAKE_API_VERSION(0, 0, 1, 0)) {}

	Renderer &Graphics::createRenderer(system::Window window) {
		return renderers.emplace_back(instance, window);
	}
}