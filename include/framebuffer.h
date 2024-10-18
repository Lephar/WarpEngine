#pragma once

#include "image.h"
#include "zero.h"

struct framebuffer {
    Image depthStencil;
    Image color;
    Image resolve;
} typedef Framebuffer;
