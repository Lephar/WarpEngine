#pragma once

#ifndef NDEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif // NDEBUG

#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbit.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <sys/mman.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL_stdinc.h>
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.h>

#include "cgltf/cgltf.h"

#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#define CGLM_FORCE_LEFT_HANDED
#include "cglm/cglm.h"
