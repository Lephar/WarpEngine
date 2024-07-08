#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Graphics {
    class Instance {
    private:
        vk::raii::Context context;

        vk::raii::Instance *instance;
#ifndef NDEBUG
        vk::raii::DebugUtilsMessengerEXT *messenger;
#endif
    public:
        Instance(const char *title, std::vector<const char *> layers, std::vector<const char *> extensions);
        ~Instance();
    };
}
