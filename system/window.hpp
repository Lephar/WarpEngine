#ifndef ZERO_CLIENT_SYSTEM_WINDOW_HPP
#define ZERO_CLIENT_SYSTEM_WINDOW_HPP

#include "base.hpp"

namespace zero::system {
	class Window {
	public: //TODO: Change visibility and add callback fields
		const char* title;
		uint32_t width;
		uint32_t height;
		Window(const char* title, uint16_t width, uint16_t height);
	};
}

#endif //ZERO_CLIENT_SYSTEM_WINDOW_HPP
