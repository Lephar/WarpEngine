#pragma once

#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace Graphics {
    class Queue {
    private:
        vk::raii::Queue *queue;
        vk::raii::CommandPool *commandPool;
    public:
        static uint32_t chooseQueueFamily(std::vector<vk::QueueFamilyProperties> queueFamilyPropertiesList, vk::QueueFlags requiredFlags);
    };
}
