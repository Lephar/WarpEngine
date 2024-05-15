#include "Zero.hpp"
#include "System/System.hpp"

int main(int argc, char* args[])
{
	static_cast<void>(argc);
	static_cast<void>(args);

	System::initialize("Zero", 800, 600);

	return EXIT_SUCCESS;
}
