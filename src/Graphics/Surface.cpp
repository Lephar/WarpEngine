#include "Graphics/Surface.hpp"

namespace Graphics {
    Surface::Surface(vk::raii::SurfaceKHR surface) : surface{std::move(surface)} {

    }
}
