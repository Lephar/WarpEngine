#include "System.hpp"
#include "Graphics.hpp"

int main(int argc, char* args[]) {
	System system("Zero", 800, 600);
	Graphics graphics(system);

	//system.mainLoop();

	return 0;
}