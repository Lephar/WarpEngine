#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Graphics {
    class Device;

    class Instance {
    private:
        vk::raii::Instance *instance;
#ifndef NDEBUG
        vk::raii::DebugUtilsMessengerEXT *messenger;
#endif
        std::vector<Device *> devices;
        size_t defaultDeviceIndex;

    public:
        Instance(vk::raii::Context& context, const char *title, std::vector<const char *> layers, std::vector<const char *> extensions);
        vk::raii::Instance *getInstanceHandle();
        Device *getDevice(size_t index);
        Device *getDefaultDevice();
        ~Instance();
    };
}
