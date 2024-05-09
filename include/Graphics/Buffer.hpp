#pragma once

#include "Graphics.hpp"
#include "Memory.hpp"
#include "Image.hpp"

namespace Graphics {
	class Buffer {
	public:
		Memory *memory;

		bool created;
		bool bound;

		vk::Buffer buffer;
		vk::DeviceSize size;
		vk::DeviceSize offset;

		void create(vk::DeviceSize size, vk::BufferUsageFlags usage);
		void bindMemory(Memory *memory);
		void copy(Buffer &destination);
		void copyToImage(Image &destination);
		void destroy();
	};
}
