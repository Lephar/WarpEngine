#include <Graphics.hpp>

int main(int argc, char* args[])
{
	static_cast<void>(argc);
	static_cast<void>(args);

	Graphics graphics{"Zero", 1280, 720};
	graphics.draw();

	return EXIT_SUCCESS;
}
