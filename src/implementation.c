// NOTICE: We define these again to use here since we cannot include pch.h here
#ifndef NDEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

#ifdef DEBUG
// #define STBI_FAILURE_USERMSG // WARN: Causes a compilation warning in stbi
#else
#define STBI_NO_FAILURE_STRINGS
#endif

#define STBI_MALLOC(sz)       malloc(sz*2) // To generate mipmaps in place without extra allocation
#define STBI_REALLOC(p,newsz) realloc(p,newsz*2)
#define STBI_FREE(p)          free(p)

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize2.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf/cgltf.h"
