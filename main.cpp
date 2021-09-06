#include "system/system.hpp"
#include "graphics/graphics.hpp"

int main() {
	zero::system::System system;
	auto &window = system.createWindow("Zero", 800, 600);

	zero::graphics::Graphics graphics;
	auto &renderer = graphics.createRenderer(window);

	system.mainLoop();

	return 0;
}
