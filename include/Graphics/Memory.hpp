#pragma once

#include "Graphics.hpp"

class Memory {
private:
    Renderer *owner;

    bool allocated;

    uint32_t type;
    vk::DeviceMemory memory;
    vk::DeviceSize size;
    vk::DeviceSize offset;

public:
    void allocate(Renderer *owner, vk::PhysicalDeviceMemoryProperties memoryProperties, uint32_t filter, vk::MemoryPropertyFlags properties, vk::DeviceSize size);
    void align(vk::DeviceSize alignment);
    vk::DeviceSize bind(vk::Buffer &buffer);
    vk::DeviceSize bind(vk::Image &image);
    void destroy();
};