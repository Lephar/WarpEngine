#include "platformBase.h"
#include "renderSystem.h"
#include "networkSystem.h"

void gameLoop(void) {

}

int main(void) {
	createWindow();
	startDaemon();

    initializeRenderer();
	gameLoop();
	destroyRenderer();

	stopDaemon();
	destroyWindow();
}
