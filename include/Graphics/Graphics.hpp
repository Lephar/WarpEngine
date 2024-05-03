#pragma once

namespace Graphics {
    class Renderer;
    class DeviceInfo;
    class Memory;
    class Image;
    class Buffer;
    class Framebuffer;

	void initialize();

	vk::Instance getInstance();
	vk::Device getDevice();

	vk::CommandBuffer beginSingleTimeCommand();
	void endSingleTimeCommand(vk::CommandBuffer commandBuffer);

	void draw(void (*render)(void) = nullptr);

	void destroy();
}