module;

#include "Graphics.hpp"

module Engine:Graphics;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace Engine::Graphics {
	void createCore();
	void createDevice();
	void createObjects();
	void destroyObjects();
	void destroyDevice();
	void destroyCore();

	void initialize() {
		createCore();
		createDevice();
		createObjects();
	}

	void terminate() {
		destroyObjects();
		destroyDevice();
		destroyCore();
	}
}
