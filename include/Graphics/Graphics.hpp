#pragma once

#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace Graphics {
    class Device;

    vk::raii::Instance *initialize(const char *title, std::vector<const char *> layers, std::vector<const char *> extensions);
    Device *getDevice(size_t index);
    Device *getDefaultDevice(void);
    void destroy(void);
}
