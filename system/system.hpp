#ifndef ZERO_CLIENT_SYSTEM_HPP
#define ZERO_CLIENT_SYSTEM_HPP

#include "header.hpp"
#include "window.hpp"

namespace zero::system {
	class System {
	private:
		xcb_connection_t *connection;
		xcb_screen_t *screen;
		std::unordered_map<uint32_t, Window> windows;
		xcb_atom_t requestAtom(const char* message);
		void processEvents(void);
	public:
		System();
		xcb_connection_t* getConnection(void);
		xcb_window_t createWindow(const char* title, uint16_t width, uint16_t height);
		void mainLoop(void);
		void destroyWindow(uint32_t index);
		~System(void);
	};
}

#endif //ZERO_CLIENT_SYSTEM_HPP
