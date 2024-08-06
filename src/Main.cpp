#include "System/System.hpp"
#include "System/Window.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Instance.hpp"
#include "Graphics/Device.hpp"

int main(int argc, char* args[])
{
    static_cast<void>(argc);
    static_cast<void>(args);

    auto window = System::initialize("Zero", 800, 600);
    auto instance = Graphics::initialize("Zero", std::vector<const char *>{}, window->getExtensions());
    auto device = Graphics::getDefaultDevice();

    auto surface = window->createSurface(**instance->getInstance());
    device->registerSurface(vk::raii::SurfaceKHR{*instance->getInstance(), surface});

    Graphics::destroy();
    System::quit();

    return EXIT_SUCCESS;
}
