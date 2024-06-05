#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Graphics {
    class Queue;

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
    public:
        Device(vk::raii::PhysicalDevice physicalDevice);
        vk::raii::Device *operator*();
        ~Device(void);
    };
}
