#ifndef ZERO_CLIENT_PLATFORM_BASE_H
#define ZERO_CLIENT_PLATFORM_BASE_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

void createWindow(void);

VkSurfaceKHR createSurface(VkInstance);

const char **getExtensions(uint32_t *);

#ifndef NDEBUG

PFN_vkCreateDebugUtilsMessengerEXT getMessengerCreator(VkInstance);

PFN_vkDestroyDebugUtilsMessengerEXT getMessengerDestroyer(VkInstance);

#endif //NDEBUG

void destroyWindow(void);

#endif //ZERO_CLIENT_PLATFORM_BASE_H
