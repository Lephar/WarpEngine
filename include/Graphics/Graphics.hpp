#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace Graphics {
    class Instance;
    class Device;

    Instance *initialize(const char *title, std::vector<const char *> layers, std::vector<const char *> extensions);
    Instance *getInstance();
    Device *getDevice(size_t index);
    Device *getDefaultDevice();
    void registerSurface(VkSurfaceKHR surface);
    void destroy();
}
