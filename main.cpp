#include "system/system.hpp"

int main() {
	zero::system::System system;

	auto connection = system.getConnection();
	auto window1 = system.createWindow("Window 1", 800, 600);
	auto window2 = system.createWindow("Window 2", 320, 240);
	auto window3 = system.createWindow("Window 3", 640, 480);
	system.destroyWindow(window2);

	system.mainLoop();

	return 0;
}
