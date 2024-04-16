#pragma once

#include "Graphics.hpp"

class Image {
    friend class Buffer;

private:
    Renderer *owner;
    Memory *memory;

    uint32_t width;
    uint32_t height;
    vk::Format format;
    vk::ImageUsageFlags usage;
    vk::SampleCountFlagBits samples;
    vk::ImageAspectFlags aspects;
    uint32_t mips;

    vk::Image image;
    vk::ImageView view;

public:
    void initialize(Renderer *owner);
    void create(uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags usage, vk::SampleCountFlagBits samples, uint32_t mips);
    void bindMemory(Memory *memory);
    void createView(vk::Format format, vk::ImageAspectFlags aspects, uint32_t mips);
    void transitionLayout(vk::ImageAspectFlags aspects, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
};