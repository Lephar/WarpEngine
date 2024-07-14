#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Graphics {
    class Queue {
    private:
        uint32_t queueFamilyIndex;
        uint32_t queueIndex;

        vk::raii::Queue queue;
        vk::raii::CommandPool commandPool;
        vk::raii::CommandBuffer commandBuffer;
    public:
        Queue(vk::raii::Device *device, uint32_t queueFamilyIndex, uint32_t queueIndex);
        static uint32_t chooseQueueFamily(std::vector<vk::QueueFamilyProperties> queueFamilyPropertiesList, vk::QueueFlags requiredFlags);
    };
}
