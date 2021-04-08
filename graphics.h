#ifndef GRAPHICS_H

#include <vulkan/vulkan.h>

void createInstance(uint32_t, const char **, VkInstance *);

void bindFunctions(PFN_vkCreateDebugUtilsMessengerEXT, PFN_vkDestroyDebugUtilsMessengerEXT);

void createMessenger(void);

void bindSurface(VkSurfaceKHR);

void startRenderer(void);

void destroyRenderer(void);

#endif
