#ifndef GRAPHICS_H

#include <vulkan/vulkan.h>

void createInstance(uint32_t, const char **, VkInstance *);

void bindSurface(VkSurfaceKHR);

void startRenderer(void);

void destroyRenderer(void);

#endif
