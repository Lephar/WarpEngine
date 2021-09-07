#ifndef ZERO_CLIENT_GRAPHICS_HPP
#define ZERO_CLIENT_GRAPHICS_HPP

#include "base.hpp"

namespace zero::graphics {
    class Graphics {
    private:
        vk::Instance instance;
#ifndef NDEBUG
        vk::DispatchLoaderDynamic loader;
        vk::DebugUtilsMessengerEXT messenger;
#endif //NDEBUG

        vk::PhysicalDevice physicalDevice;
        uint32_t transferQueueFamily{};
        uint32_t graphicsQueueFamily{};

        vk::Device device;
        vk::Queue transferQueue;
        vk::Queue graphicsQueue;

        void createInstance();

        void selectPhysicalDevice();

        uint32_t selectQueueFamily(vk::QueueFlags queueFlags);

        void selectQueueFamilies();

        void createDevice();

    public:
        Graphics();

        ~Graphics();
    };
}

#endif //ZERO_CLIENT_GRAPHICS_HPP
