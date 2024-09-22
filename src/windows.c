#include "window.h"

SDL_bool systemInitialized = SDL_FALSE;
SDL_bool windowCreated = SDL_FALSE;
SDL_bool surfaceCreated = SDL_FALSE;
SDL_bool drawing = SDL_FALSE;

PFN_vkGetInstanceProcAddr getInstanceProcAddr;
const char *title = NULL;
SDL_Window *window = NULL;
VkExtent2D extent = {};
uint32_t instanceExtensionCount = 0;
const char **instanceExtensionNames = NULL;

extern VkInstance instance;
extern VkSurfaceKHR surface;

void initialize() {
    assert(!systemInitialized);

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Vulkan_LoadLibrary(NULL);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    
    getInstanceProcAddr = SDL_Vulkan_GetVkGetInstanceProcAddr();

    systemInitialized = SDL_TRUE;
}

void createWindow(const char *name, int32_t width, int32_t height) {
    assert(systemInitialized && !windowCreated);

    title = name;

    window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_VULKAN);
    SDL_Vulkan_GetDrawableSize(window, (int32_t *)&extent.width, (int32_t *)&extent.height);

    SDL_Vulkan_GetInstanceExtensions(window, &instanceExtensionCount, NULL);

    instanceExtensionNames = malloc(instanceExtensionCount * sizeof(const char *));
    SDL_Vulkan_GetInstanceExtensions(window, &instanceExtensionCount, instanceExtensionNames);
    
    windowCreated = SDL_TRUE;
}

void createSurface() {
    assert(windowCreated && !surfaceCreated);

    SDL_Vulkan_CreateSurface(window, instance, &surface);

    surfaceCreated = SDL_TRUE;
}

void draw(void (*render)(void)) {
    assert(surfaceCreated && !drawing);
    
    drawing = SDL_TRUE;

    SDL_Event event;

    while (true) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                drawing = SDL_FALSE;
                break;
            }
        }

        if(!drawing) {
            break;
        }

        if(render) {
            render();
        }
    }
}

void destroySurface() {
    assert(surfaceCreated);

    vkDestroySurfaceKHR(instance, surface, NULL);

    surfaceCreated = SDL_FALSE;
}

void destroyWindow() {
    assert(!surfaceCreated && windowCreated);

    free(instanceExtensionNames);
    instanceExtensionCount = 0;
    
    SDL_DestroyWindow(window);
    window = NULL;

    extent.height = 0;
    extent.width = 0;

    windowCreated = SDL_FALSE;
}

void quit() {
    assert(!windowCreated && systemInitialized);

    getInstanceProcAddr = NULL;
    SDL_Vulkan_UnloadLibrary();
    SDL_Quit();

    systemInitialized = SDL_FALSE;
}
