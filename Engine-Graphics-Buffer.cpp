module;

#include "Engine-Graphics.hpp"

module Engine:Graphics;

namespace Engine::Graphics {
	extern vk::Device device;

	Buffer::Buffer(vk::BufferUsageFlags usageFlags, vk::DeviceSize bufferSize, Memory& bufferMemory)
		: size(bufferSize), memory(bufferMemory.getMemoryHandle()) {
		vk::BufferCreateInfo bufferInfo{
			.size = size,
			.usage = usageFlags,
			.sharingMode = vk::SharingMode::eExclusive
		};

		buffer = device.createBuffer(bufferInfo);
		offset = bufferMemory.bindBuffer(buffer);
	}
}
