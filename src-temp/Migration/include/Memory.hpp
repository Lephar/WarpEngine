#pragma once

#include "Graphics.hpp"

namespace Graphics {
    class Memory {
    public:
        bool allocated;

        uint32_t type;
        vk::DeviceMemory memory;
        vk::DeviceSize size;
        vk::DeviceSize offset;

        void allocate(vk::PhysicalDeviceMemoryProperties memoryProperties, uint32_t filter, vk::MemoryPropertyFlags properties, vk::DeviceSize size);
        void align(vk::DeviceSize alignment);
        vk::DeviceSize bind(vk::Buffer &buffer);
        vk::DeviceSize bind(vk::Image &image);
        void destroy();
    };
}
