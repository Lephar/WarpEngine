#include "System/System.hpp"
#include "System/Window.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"

int main(int argc, char* args[])
{
    static_cast<void>(argc);
    static_cast<void>(args);

    auto window = System::initialize("Zero", 800, 600);
    auto instance = Graphics::initialize("Zero", std::vector<const char *>{}, window->getExtensions());

    Graphics::destroy();

    return EXIT_SUCCESS;
}
