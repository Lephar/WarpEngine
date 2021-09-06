#include "window.hpp"

namespace zero::system {
	Window::Window(xcb_connection_t *connection, xcb_window_t window, const char *title, uint16_t width,
				   uint16_t height) : connection(connection), window(window), title(title), width(width),
									  height(height) {}
}
