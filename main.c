#include "base.h"
#include "graphics.h"
#include "network.h"

void bindContext(void) {
	uint32_t extensionCount;
	const char **extensionNames;
	VkInstance instance;
	PFN_vkCreateDebugUtilsMessengerEXT messengerCreator;
	PFN_vkDestroyDebugUtilsMessengerEXT messengerDestroyer;
	VkSurfaceKHR surface;

	getExtensions(&extensionCount, &extensionNames);
	createInstance(extensionCount, extensionNames, &instance);

	loadFunctions(instance, &messengerCreator, &messengerDestroyer);
	bindFunctions(messengerCreator, messengerDestroyer);
	createMessenger();

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
