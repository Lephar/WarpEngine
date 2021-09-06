#include "system.hpp"

namespace zero::system {
	uint32_t mask;
	uint32_t flags[2];
	xcb_atom_t protocol;
	xcb_atom_t destroyEvent;

	xcb_atom_t System::requestAtom(const char *message) {
		xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 0, strlen(message), message);
		xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, NULL);
		xcb_atom_t atom = reply->atom;
		free(reply);

		return atom;
	}

	System::System(void) {
		connection = xcb_connect(NULL, NULL);
		const xcb_setup_t *setup = xcb_get_setup(connection);
		screen = xcb_setup_roots_iterator(setup).data;

		mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
		flags[0] = screen->black_pixel;
		flags[1] = XCB_EVENT_MASK_EXPOSURE |
				XCB_EVENT_MASK_STRUCTURE_NOTIFY |
				XCB_EVENT_MASK_ENTER_WINDOW |
				XCB_EVENT_MASK_LEAVE_WINDOW |
				XCB_EVENT_MASK_POINTER_MOTION |
				XCB_EVENT_MASK_BUTTON_PRESS |
				XCB_EVENT_MASK_BUTTON_RELEASE |
				XCB_EVENT_MASK_KEY_PRESS |
				XCB_EVENT_MASK_KEY_RELEASE;

		protocol = requestAtom("WM_PROTOCOLS");
		destroyEvent = requestAtom("WM_DELETE_WINDOW");
	}

	xcb_connection_t *System::getConnection(void) {
		return connection;
	}

	xcb_window_t System::createWindow(const char *title, uint16_t width, uint16_t height) {
		xcb_window_t window = xcb_generate_id(connection);
		xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, width, height, 0,
						  XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, mask, flags);

		xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, CHAR_BIT,
							strlen(title), title);
		xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, protocol, XCB_ATOM_ATOM,
							sizeof(xcb_atom_t) * CHAR_BIT, 1, &destroyEvent);

		xcb_map_window(connection, window);
		xcb_flush(connection);

		windows.emplace(std::piecewise_construct, std::forward_as_tuple(window),
						std::forward_as_tuple(title, width, height));

		return window;
	}

	void System::processEvents(void) {
		while (1) {
			xcb_generic_event_t *event = xcb_poll_for_event(connection);

			if (!event)
				break;

			uint8_t response = event->response_type & ~0x80; // Why is this the norm?..

			switch (response) {
				case XCB_CONFIGURE_NOTIFY: {
					xcb_configure_notify_event_t *configureNotify = (xcb_configure_notify_event_t *) event;
					xcb_window_t index = configureNotify->window;
					Window &window = windows.at(index);

					if (configureNotify->width > 0 && configureNotify->height > 0 &&
						(configureNotify->width != window.width || configureNotify->height != window.height)) {
						window.width = configureNotify->width;
						window.height = configureNotify->height;

						// Resize callback here
					}

					break;
				}
				case XCB_CLIENT_MESSAGE: {
					xcb_client_message_event_t *clientMessage = (xcb_client_message_event_t *) event;
					xcb_window_t index = clientMessage->window;

					if (clientMessage->format == sizeof(destroyEvent) * CHAR_BIT &&
						clientMessage->data.data32[0] == destroyEvent) {
						// Destroy callback here

						destroyWindow(index);
					}

					break;
				}
			}

			std::free(event);
		}
	}

	void System::mainLoop(void) {
		while (!windows.empty()) {
			processEvents();

			//Draw loop here
		}
	}

	void System::destroyWindow(uint32_t index) {
		xcb_destroy_window(connection, index);
		xcb_flush(connection);
		windows.erase(index);
	}

	System::~System(void) {
		for (auto &window: windows)
			xcb_destroy_window(connection, window.first);
		xcb_flush(connection); // Is it necessary before disconnect?
		xcb_disconnect(connection);
		windows.clear();
	}
}