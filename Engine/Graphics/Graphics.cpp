#include "Graphics.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace Engine::Graphics {

	void initialize() {
		createObjects();
	}

	void terminate() {
		destroyObjects();
	}
}
