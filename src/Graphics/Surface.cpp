#include "Graphics/Surface.hpp"

namespace Graphics {
    Surface::Surface(vk::raii::SurfaceKHR surface, vk::Extent2D extent) : surface{std::move(surface)}, extent{extent} {

    }
}
