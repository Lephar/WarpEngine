#include "Engine.hpp"

namespace Engine {
	void run(const char* title, unsigned int width, unsigned int height) {
		System::initialize(title, width, height);

		auto loader = System::getLoader();
		auto extensions = System::getExtensions();

		auto instance = Graphics::createInstance(title, loader, extensions);
		auto surface = System::createSurface(instance);

		Graphics::registerSurface(surface);
		Graphics::createDevice();

		Graphics::initialize();

		//System::draw();

		Graphics::terminate();
		System::terminate();
	}
}
