#ifndef ZERO_CLIENT_RENDER_SYSTEM_CORE_H
#define ZERO_CLIENT_RENDER_SYSTEM_CORE_H

#include <vulkan/vulkan.h>

struct core {
    VkInstance instance;
#ifndef NDEBUG
    VkDebugUtilsMessengerEXT messenger;
#endif //NDEBUG
    VkSurfaceKHR surface;
};

typedef struct core Core;

Core createCore(void);
void destroyCore(Core core);

#endif //ZERO_CLIENT_RENDER_SYSTEM_CORE_H
