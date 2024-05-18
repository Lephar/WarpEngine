#include "Zero.hpp"
#include "System/System.hpp"
#include "System/Window.hpp"
#include "Graphics/Graphics.hpp"

int main(int argc, char* args[])
{
	static_cast<void>(argc);
	static_cast<void>(args);

	const char *title = "Zero";

	System::Window window = System::initialize(title, 800, 600);
	Graphics::initialize(title, System::getLoader(), System::getExtensions());

	return EXIT_SUCCESS;
}
