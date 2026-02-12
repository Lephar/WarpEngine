#include "system.h"

#include "window.h"
#include "surface.h"

#include "logger.h"

int32_t threadCount;

char const *applicationName;
char dataPath[PATH_MAX];

PFN_vkGetInstanceProcAddr systemFunctionLoader;

void initializeSystem() {
    applicationName = "Warp";

    windowWidth  = 1280;
    windowHeight = 720;

    threadCount = get_nprocs();
    debug("Online thread count: %d", threadCount);

    // NOTICE: RenderDoc doesn't support Wayland yet
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "wayland");
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    debug("Video system initialized with %s driver", SDL_GetCurrentVideoDriver());

    char const *basePath = SDL_GetBasePath(); // NOTICE: Already ends with /
    snprintf(dataPath, PATH_MAX, "%s%s", basePath, DEBUG ? "../data" : "data");
    debug("Data path: %s", dataPath);

    SDL_Vulkan_LoadLibrary(nullptr);
    systemFunctionLoader = (PFN_vkGetInstanceProcAddr) SDL_Vulkan_GetVkGetInstanceProcAddr();
    debug("System function loader initialized", SDL_GetCurrentVideoDriver());

    uint32_t baseSurfaceInstanceExtensionCount = 0;
    char const *const *baseSurfaceInstanceExtensions = SDL_Vulkan_GetInstanceExtensions(&baseSurfaceInstanceExtensionCount);

    char const *maintenanceSurfaceInstanceExtensions[] = {
        VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
        VK_EXT_SURFACE_MAINTENANCE_1_EXTENSION_NAME,
        VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME,
    };

    uint32_t maintenanceSurfaceInstanceExtensionCount = sizeof(maintenanceSurfaceInstanceExtensions) / sizeof(char const *);

    surfaceInstanceExtensionCount = baseSurfaceInstanceExtensionCount + maintenanceSurfaceInstanceExtensionCount;
    surfaceInstanceExtensions = malloc(surfaceInstanceExtensionCount * sizeof(char const *));

    memcpy(surfaceInstanceExtensions, baseSurfaceInstanceExtensions, baseSurfaceInstanceExtensionCount * sizeof(char const *));
    memcpy(surfaceInstanceExtensions + baseSurfaceInstanceExtensionCount, maintenanceSurfaceInstanceExtensions, maintenanceSurfaceInstanceExtensionCount * sizeof(char const *));

    char const *baseSurfaceDeviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    uint32_t baseSurfaceDeviceExtensionCount = sizeof(baseSurfaceDeviceExtensions) / sizeof(char const *);

    char const *maintenanceSurfaceDeviceExtensions[] = {
        VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME,
        VK_EXT_PRESENT_MODE_FIFO_LATEST_READY_EXTENSION_NAME,
    };

    const uint32_t maintenanceSurfaceDeviceExtensionCount = sizeof(maintenanceSurfaceDeviceExtensions) / sizeof(char const *);

    surfaceDeviceExtensionCount = baseSurfaceDeviceExtensionCount + maintenanceSurfaceDeviceExtensionCount;
    surfaceDeviceExtensions = malloc(surfaceDeviceExtensionCount * sizeof(char const *));

    memcpy(surfaceDeviceExtensions, baseSurfaceDeviceExtensions, baseSurfaceDeviceExtensionCount * sizeof(char const *));
    memcpy(surfaceDeviceExtensions + baseSurfaceDeviceExtensionCount, maintenanceSurfaceDeviceExtensions, maintenanceSurfaceDeviceExtensionCount * sizeof(char const *));

    VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT *presentModeFifoLatestReadyFeatures = malloc(sizeof(VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT));
    presentModeFifoLatestReadyFeatures->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_MODE_FIFO_LATEST_READY_FEATURES_EXT;
    presentModeFifoLatestReadyFeatures->pNext = nullptr;
    presentModeFifoLatestReadyFeatures->presentModeFifoLatestReady = VK_TRUE;

    VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT *swapchainMaintenance1Features = malloc(sizeof(VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT));
    swapchainMaintenance1Features->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_EXT;
    swapchainMaintenance1Features->pNext = presentModeFifoLatestReadyFeatures;
    swapchainMaintenance1Features->swapchainMaintenance1 = VK_TRUE;

    surfaceDeviceFeatures = swapchainMaintenance1Features;
}

void quitSystem() {
    VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT *swapchainMaintenance1Features = surfaceDeviceFeatures;
    VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT *presentModeFifoLatestReadyFeatures = swapchainMaintenance1Features->pNext;
    assert(presentModeFifoLatestReadyFeatures->pNext == nullptr);

    free(presentModeFifoLatestReadyFeatures);
    free(swapchainMaintenance1Features);
    surfaceDeviceFeatures = nullptr;

    surfaceDeviceExtensionCount = 0;
    free(surfaceDeviceExtensions);

    surfaceInstanceExtensionCount = 0;
    free(surfaceInstanceExtensions);

    SDL_Quit();

    debug("System quit");
}
