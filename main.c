#include "base.h"
#include "network.h"

void gameLoop(void) {

}

int main() {
	initializeBase();
	startDaemon();

	stopDaemon();
	clearBase();
}
