module Engine;

import :System;
import :Graphics;

namespace Engine {
	void run(const char* title, unsigned int width, unsigned int height) {
		System::initialize(title, width, height);
		Graphics::initialize();

		//System::draw();

		Graphics::terminate();
		System::terminate();
	}
}
