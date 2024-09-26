#include "window.h"
#include "helper.h"

SDL_bool systemInitialized = SDL_FALSE;
SDL_bool windowCreated = SDL_FALSE;
extern SDL_bool surfaceCreated;
SDL_bool drawing = SDL_FALSE;

PFN_vkGetInstanceProcAddr getInstanceProcAddr;
const char *title = NULL;
SDL_Window *window = NULL;
VkExtent2D extent = {};
uint32_t requiredInstanceExtensionCount = 0;
const char **requiredInstanceExtensionNames = NULL;

extern VkInstance instance;
extern VkSurfaceKHR surface;

void initialize() {
    assert(!systemInitialized);

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Vulkan_LoadLibrary(NULL);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    getInstanceProcAddr = SDL_Vulkan_GetVkGetInstanceProcAddr();

    systemInitialized = SDL_TRUE;
    debug("System initialized");
}

void createWindow(const char *name, int32_t width, int32_t height) {
    assert(systemInitialized && !windowCreated);

    title = name;

    window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_VULKAN);
    SDL_Vulkan_GetDrawableSize(window, (int32_t *)&extent.width, (int32_t *)&extent.height);

    SDL_Vulkan_GetInstanceExtensions(window, &requiredInstanceExtensionCount, NULL);

    requiredInstanceExtensionNames = malloc(requiredInstanceExtensionCount * sizeof(const char *));
    SDL_Vulkan_GetInstanceExtensions(window, &requiredInstanceExtensionCount, requiredInstanceExtensionNames);

    windowCreated = SDL_TRUE;
    debug("Window created");
}

void draw(void (*render)()) {
    assert(surfaceCreated && !drawing);

    drawing = SDL_TRUE;
    debug("Draw loop started");

    while (true) {
        SDL_Event event;

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

    debug("Draw loop ended");
}

void destroyWindow() {
    assert(!surfaceCreated && windowCreated);

    free(requiredInstanceExtensionNames);
    requiredInstanceExtensionCount = 0;

    SDL_DestroyWindow(window);
    window = NULL;

    extent.height = 0;
    extent.width = 0;

    windowCreated = SDL_FALSE;
    debug("Window destroyed");
}

void quit() {
    assert(!windowCreated && systemInitialized);

    getInstanceProcAddr = NULL;
    SDL_Vulkan_UnloadLibrary();
    SDL_Quit();

    systemInitialized = SDL_FALSE;
    debug("System quitted");
}
