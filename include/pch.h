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
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbit.h>
#include <string.h>
#include <syslog.h>
#include <limits.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <uuid/uuid.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/sysinfo.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_joystick.h>
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.h>

#include "stb/stb_image.h"
#include "stb/stb_image_resize2.h"
#include "ktx/ktx.h"
#include "cgltf/cgltf.h"
#include "cglm/call.h"
#include "cglm/call/clipspace/view_rh_zo.h"
#include "cglm/call/clipspace/persp_rh_zo.h"
