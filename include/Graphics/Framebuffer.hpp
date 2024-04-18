#pragma once

#include "Graphics.hpp"
#include "Image.hpp"

class Framebuffer {
    Renderer *owner;

    size_t size;

    std::vector<Image> depthStencil;
    std::vector<Image> color;
    std::vector<Image> resolve;

    bool created;

    void create(Renderer *owner, Memory *memory, uint32_t width, uint32_t height, size_t size, vk::SampleCountFlagBits samples, vk::Format depthStencilFormat, vk::Format colorFormat);
    vk::Image getDepthStencilImage(size_t index);
    vk::Image getColorImage(size_t index);
    vk::Image getResolveImage(size_t index);
    vk::ImageView getDepthStencilView(size_t index);
    vk::ImageView getColorView(size_t index);
    vk::ImageView getResolveView(size_t index);
    void destroy();
};
