#include "Base.hpp"
#include "System.hpp"

int main(int argc, char* args[]) {
	System system("Zero", 800, 600);

	system.mainLoop();

	return 0;
}
