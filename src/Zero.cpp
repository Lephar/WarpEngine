#include "Zero.hpp"
#include "System/System.hpp"

int main(int argc, char* args[])
{
	static_cast<void>(argc);
	static_cast<void>(args);

	System::initialize();

	return EXIT_SUCCESS;
}
