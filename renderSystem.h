#ifndef ZERO_CLIENT_RENDER_SYSTEM_H
#define ZERO_CLIENT_RENDER_SYSTEM_H

#include <vulkan/vulkan.h>

void createInstance(uint32_t, const char **, VkInstance *);

void bindSurface(VkSurfaceKHR);

void prepareRenderer(void);

void destroyRenderer(void);

#endif
