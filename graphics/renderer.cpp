#include "renderer.hpp"

namespace zero::graphics {
	Renderer::Renderer(Instance &instance, system::Window &window) : window(window), instance(instance), surface(instance, window),
																	 internals(instance, surface), details(internals) {}
}