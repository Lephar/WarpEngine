#include "base.h"
#include "graphics.h"
#include "network.h"

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
	startRenderer();
	gameLoop();
	destroyRenderer();

	stopDaemon();
	destroyWindow();
}
