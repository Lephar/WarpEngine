#pragma once

#include "Graphics.hpp"

class Buffer {
private:
	Renderer *owner;
	Memory *memory;

	vk::Buffer buffer;
	vk::DeviceSize size;
	vk::DeviceSize offset;

public:
	void initalize(Renderer *owner);
	void create(vk::DeviceSize size, vk::BufferUsageFlags usage);
	void bindMemory(Memory *memory);
	void copy(Buffer &destination);
	void copyToImage(Image &destination);
	void destroy();
};