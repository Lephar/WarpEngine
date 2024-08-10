#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Graphics {
    class Queue;
    class Surface;

    class Device {
    private:
        vk::raii::PhysicalDevice physicalDevice;

        vk::PhysicalDeviceProperties properties;
        vk::PhysicalDeviceFeatures features;
        vk::PhysicalDeviceMemoryProperties memoryProperties;
        std::vector<vk::QueueFamilyProperties> queueFamilyPropertiesList;

        vk::raii::Device *device;

        Queue *graphicsQueue;
        Queue *computeQueue ;
        Queue *transferQueue;

        std::vector<Surface *> surfaces;

    public:
        Device(vk::raii::PhysicalDevice physicalDevice);
        bool isDiscrete();
        void registerSurface(vk::raii::SurfaceKHR surface, vk::Extent2D extent);
        Surface *getSurface(size_t index);
        Surface *getDefaultSurface();
        ~Device();
    };
}
