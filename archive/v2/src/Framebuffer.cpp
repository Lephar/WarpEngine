#include "Graphics/Framebuffer.hpp"
#include "Graphics/Renderer.hpp"

void Framebuffer::create(Renderer *owner, Memory *memory, uint32_t width, uint32_t height, size_t size, vk::SampleCountFlagBits samples, vk::Format depthStencilFormat, vk::Format colorFormat) {
    this->owner = owner;
    this->size = size;

    depthStencil.resize(size);
    color.resize(size);
    resolve.resize(size);

    for(size_t index = 0; index < size; index++) {
        auto &depthStencilImage = depthStencil.at(index);
        auto &colorImage = color.at(index);
        auto &resolveImage = resolve.at(index);

        depthStencilImage.create(owner, width, height, depthStencilFormat, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, samples, 1);
        colorImage.create(owner, width, height, colorFormat, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor, samples, 1);
        resolveImage.create(owner, width, height, depthStencilFormat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc, vk::ImageAspectFlagBits::eColor, vk::SampleCountFlagBits::e1, 1);

        depthStencilImage.bindMemory(memory);
        colorImage.bindMemory(memory);
        resolveImage.bindMemory(memory);
        
        depthStencilImage.createView();
        colorImage.createView();
        resolveImage.createView();

        resolveImage.transitionLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);
    }

    created = true;
}

vk::Image Framebuffer::getDepthStencilImage(size_t index) {
    return depthStencil.at(index).image;
}

vk::Image Framebuffer::getColorImage(size_t index) {
    return color.at(index).image;
}

vk::Image Framebuffer::getResolveImage(size_t index) {
    return resolve.at(index).image;
}

vk::ImageView Framebuffer::getDepthStencilView(size_t index) {
    return depthStencil.at(index).view;
}

vk::ImageView Framebuffer::getColorView(size_t index) {
    return color.at(index).view;
}

vk::ImageView Framebuffer::getResolveView(size_t index) {
    return resolve.at(index).view;
}

void Framebuffer::destroy() {
    if(!created)
        return;

    for(size_t index = 0; index < size; index++) {
        resolve.at(index).destroy();
        color.at(index).destroy();
        depthStencil.at(index).destroy();
    }

    resolve.clear();
    color.clear();
    depthStencil.clear();

    size = 0;
    owner = nullptr;

    created = false;
}
