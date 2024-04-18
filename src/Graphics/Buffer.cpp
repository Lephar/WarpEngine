#include "Graphics/Buffer.hpp"
#include "Graphics/Renderer.hpp"

void Buffer::create(Renderer *owner, vk::DeviceSize size, vk::BufferUsageFlags usage) {
	this->owner = owner;
    this->size = size;
    offset = 0;

	vk::BufferCreateInfo bufferInfo {
		.size = size,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive
	};

	buffer = owner->device.createBuffer(bufferInfo);

	created = true;
}

void Buffer::bindMemory(Memory *memory) {
	if(!created)
		return;

    this->memory = memory;
    offset = memory->bind(buffer);

	bound = true;
}

void Buffer::copy(Buffer &destination) {
	if(!bound)
		return;

	vk::BufferCopy bufferCopy {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = size
	};

	auto commandBuffer = owner->beginSingleTimeCommand();
	commandBuffer.copyBuffer(buffer, destination.buffer, bufferCopy);
	owner->endSingleTimeCommand(commandBuffer);
}

void Buffer::copyToImage(Image &destination) {
	if(!bound)
		return;
	
	destination.copyFromBuffer(buffer);
}

void Buffer::destroy() {
	if(!created)
		return;
	
	owner->device.destroyBuffer(buffer);

    offset = 0;
    size = 0;

    memory = nullptr;
    owner = nullptr;

	bound = false;
	created = false;
}