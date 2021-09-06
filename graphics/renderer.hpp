#ifndef ZERO_CLIENT_GRAPHICS_RENDERER_HPP
#define ZERO_CLIENT_GRAPHICS_RENDERER_HPP

#include "header.hpp"
#include "instance.hpp"
#include "surface.hpp"
#include "internals.hpp"
#include "details.hpp"

namespace zero::graphics {
	class Renderer {
	public:
	private:
		system::Window &window;
		Instance &instance;
		Surface surface;
		Internals internals;
		Details details;
	public:
		Renderer(Instance &instance, system::Window &window);
	};
}

#endif //ZERO_CLIENT_GRAPHICS_RENDERER_HPP
