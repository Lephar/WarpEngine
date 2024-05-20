#pragma once

#include "Graphics.hpp"
#include "Memory.hpp"

namespace Graphics {
    class Image {
    public:
        bool imageCreated;
        bool memoryBound;
        bool viewCreated;

        uint32_t width;
        uint32_t height;
        vk::Format format;
        vk::ImageUsageFlags usage;
        vk::ImageAspectFlags aspects;
        vk::SampleCountFlagBits samples;
        uint32_t mips;

        vk::Image image;
        vk::ImageView view;

        void create(uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspects, vk::SampleCountFlagBits samples, uint32_t mips);
        void wrap(vk::Image image, uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspects, vk::SampleCountFlagBits samples, uint32_t mips);
        void bindMemory(Memory *memory);
        void createView();
        void copyFromBuffer(vk::Buffer &source);
        void transitionLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
        void destroy();
    };
}
