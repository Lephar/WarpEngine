#include "system/system.hpp"

int main() {
	zero::system::System system;

	auto connection = system.getConnection();
	auto window = system.createWindow("Zero", 800, 600);

	system.mainLoop();

	return 0;
}
