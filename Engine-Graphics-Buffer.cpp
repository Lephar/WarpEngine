module;

#include "Engine-Graphics.hpp"

module Engine:Graphics;

namespace Engine::Graphics {
	extern vk::Device device;

	Buffer::Buffer(vk::BufferUsageFlags usageFlags, vk::DeviceSize bufferSize, Memory& bufferMemory)
		: size(bufferSize), memory(bufferMemory.getMemoryHandle()) {
		createBuffer(usageFlags);
		bindBufferMemory(bufferMemory);
	}

	void Buffer::createBuffer(vk::BufferUsageFlags usageFlags) {
		vk::BufferCreateInfo bufferInfo{
			.size = size,
			.usage = usageFlags,
			.sharingMode = vk::SharingMode::eExclusive
		};

		buffer = device.createBuffer(bufferInfo);
	}

	void Buffer::bindBufferMemory(Memory& bufferMemory) {
		vk::MemoryRequirements requirements = device.getBufferMemoryRequirements(buffer);
		offset = bufferMemory.alignOffset(requirements);
		device.bindBufferMemory(buffer, memory, offset);
	}

	void Buffer::destroy() {
		if (view)
			device.destroyBufferView(view);

		device.destroyBuffer(buffer);

		memory = nullptr;

		offset = 0;
		size = 0;
	}
}
