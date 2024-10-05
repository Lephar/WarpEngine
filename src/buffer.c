#include "buffer.h"
#include "memory.h"

extern VkDevice device;

Buffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage)
{
	Buffer buffer = {
		.bufferCreated = VK_TRUE,
		.size = size,
		.buffer = {},
		.memoryBound = VK_FALSE,
		.memoryOffset = 0,
		.memory = NULL
	};
	
	VkBufferCreateInfo bufferInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.size = size,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL
	};
	
	vkCreateBuffer(device, &bufferInfo, NULL, &buffer.buffer);

	return buffer;
}

void bindBufferMemory(Buffer *buffer, Memory *memory) {
	buffer->memory = memory;
	buffer->memoryOffset = memory->offset;
	buffer->memoryBound = VK_TRUE;
	memory->offset += buffer->size;

	vkBindBufferMemory(device, buffer->buffer, memory->memory, buffer->memoryOffset);
}
