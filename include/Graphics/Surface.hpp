#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace System {
    class Window;
}

namespace Graphics {
    class Instance;

    class Surface {
    private:
        vk::raii::SurfaceKHR surface;
    public:
        Surface(vk::raii::SurfaceKHR surface);
    };
}
