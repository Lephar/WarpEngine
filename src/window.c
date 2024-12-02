#include "window.h"
#include "helper.h"

extern char executableName[];

SDL_bool systemInitialized = SDL_FALSE;
SDL_bool windowCreated = SDL_FALSE;
extern SDL_bool surfaceCreated;
SDL_bool drawing = SDL_FALSE;

PFN_vkGetInstanceProcAddr getInstanceProcAddr;
SDL_Window *window = NULL;
VkExtent2D extent = {};
uint32_t requiredInstanceExtensionCount = 0;
const char **requiredInstanceExtensionNames = NULL;

extern VkInstance instance;
extern VkSurfaceKHR surface;

void initializeSystem() {
    assert(!systemInitialized);

    SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland,x11,windows");
    SDL_SetHint(SDL_HINT_AUDIODRIVER, "pipewire,pulseaudio,directsound");

    SDL_Init(SDL_INIT_EVERYTHING);
    debug("SDL Video Driver: %s", SDL_GetCurrentVideoDriver());
    debug("SDL Audio Driver: %s", SDL_GetCurrentAudioDriver());

    SDL_Vulkan_LoadLibrary(NULL);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    getInstanceProcAddr = SDL_Vulkan_GetVkGetInstanceProcAddr();

    systemInitialized = SDL_TRUE;
    debug("System initialized");
}

void createWindow() {
    assert(systemInitialized && !windowCreated);

    window = SDL_CreateWindow(executableName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, extent.width, extent.height, SDL_WINDOW_VULKAN);
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

void quitSystem() {
    assert(!windowCreated && systemInitialized);

    getInstanceProcAddr = NULL;
    SDL_Vulkan_UnloadLibrary();
    SDL_Quit();

    systemInitialized = SDL_FALSE;
    debug("System quitted");
}
