#ifndef ZERO_CLIENT_SYSTEM_WINDOW_HPP
#define ZERO_CLIENT_SYSTEM_WINDOW_HPP

#include "header.hpp"

namespace zero::system {
	class Window {
	public:
		const char* title;
		uint32_t width, height;
		Window(const char* title, uint16_t width, uint16_t height);
	};
}

#endif //ZERO_CLIENT_SYSTEM_WINDOW_HPP
