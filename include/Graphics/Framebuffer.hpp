#pragma once

#include "Graphics.hpp"
#include "Image.hpp"

class Framebuffer {
    Renderer *owner;
    Memory *memory;

    Image depthStencil;
    Image color;
    Image resolve;

    void initialize(Renderer *owner, Memory *memory);
    void setMemory();
    void create(uint32_t width, uint32_t height, );
};
