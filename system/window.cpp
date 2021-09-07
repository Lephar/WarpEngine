#include "window.hpp"

namespace zero::system {
    Window::Window(std::string title, uint16_t width, uint16_t height) : title(std::move(title)), width(width),
                                                                         height(height) {}
}
