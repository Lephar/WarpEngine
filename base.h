#ifndef SYSTEM_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

void createWindow(void);

void getExtensions(uint32_t *, const char ***);

void loadFunctions(VkInstance, PFN_vkCreateDebugUtilsMessengerEXT *, PFN_vkDestroyDebugUtilsMessengerEXT *);

void createSurface(VkInstance, VkSurfaceKHR *);

void destroyWindow(void);

#endif
