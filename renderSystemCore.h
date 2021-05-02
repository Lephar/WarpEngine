#ifndef ZERO_CLIENT_RENDER_SYSTEM_CORE_H
#define ZERO_CLIENT_RENDER_SYSTEM_CORE_H

#include <vulkan/vulkan.h>

struct zrCore {
    VkInstance instance;
    VkDebugUtilsMessengerEXT messenger;
    VkSurfaceKHR surface;
} typedef zrCore;

zrCore createCore(void);
void destroyCore(zrCore core);

#endif
