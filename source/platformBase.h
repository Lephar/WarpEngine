#ifndef ZERO_CLIENT_PLATFORM_BASE_H
#define ZERO_CLIENT_PLATFORM_BASE_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

void createWindow(void);

const char ** getExtensions(uint32_t *);

PFN_vkCreateDebugUtilsMessengerEXT getMessengerCreator(VkInstance);

VkSurfaceKHR createSurface(VkInstance);

PFN_vkDestroyDebugUtilsMessengerEXT getMessengerDestroyer(VkInstance);

void destroyWindow(void);

#endif
