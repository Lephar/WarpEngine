#include "renderer.hpp"

namespace zero::graphics {
    void Renderer::createSurface(xcb_connection_t *connection, xcb_window_t window) {
        vk::XcbSurfaceCreateInfoKHR surfaceInfo {
                .connection = connection,
                .window = window
        };

        surface = instance.createXcbSurfaceKHR(surfaceInfo);
    }

    Renderer::Renderer(vk::Instance &instance, vk::Device &device, xcb_connection_t *connection, xcb_window_t window,
                       system::Window &properties) : instance(instance) {
        createSurface(connection, window);
    }

    void Renderer::destroy() {
        instance.destroySurfaceKHR(surface);
    }
}