#pragma once

#ifndef NDEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stdbit.h>
#include <stdbool.h>
#include <sys/mman.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.h>

#include "ktx/ktx.h"
#include "cgltf/cgltf.h"
#include "cglm/call.h"
#include "cglm/call/clipspace/view_lh_zo.h"
#include "cglm/call/clipspace/persp_lh_zo.h"
