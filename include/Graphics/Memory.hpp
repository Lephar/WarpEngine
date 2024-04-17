#pragma once

#include "Graphics.hpp"

class Memory {
private:
    Renderer *owner;

    vk::DeviceMemory memory;

    uint32_t type;
    vk::DeviceSize size;
    vk::DeviceSize offset;

public:
    void allocate(Renderer *owner, uint32_t filter, vk::MemoryPropertyFlags properties, vk::DeviceSize size);
    void align(vk::DeviceSize alignment);
    vk::DeviceSize bind(vk::Buffer &buffer);
    vk::DeviceSize bind(vk::Image &image);
    void destroy();
};