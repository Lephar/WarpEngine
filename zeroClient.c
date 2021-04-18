#include "platformFramework.h"
#include "renderSystem.h"
#include "networkSystem.h"

void bindContext(void) {
	uint32_t extensionCount;
	const char **extensionNames;

	VkInstance instance;
	VkSurfaceKHR surface;

	getExtensions(&extensionCount, &extensionNames);
	createInstance(extensionCount, extensionNames, &instance);

	createSurface(instance, &surface);
	bindSurface(surface);
}

void gameLoop(void) {

}

int main(void) {
	createWindow();
	startDaemon();

	bindContext();
    prepareRenderer();
	gameLoop();
	destroyRenderer();

	stopDaemon();
	destroyWindow();
}
