#include "Graphics.hpp"

namespace Engine::Graphics {
	extern PhysicalDevice physicalDevice;
	extern vk::Device device;

	Memory sharedMemory;
	Memory deviceMemory;

	Buffer sharedBuffer;
	Buffer deviceBuffer;

	bool isMemoryPropertyFlagRedundant(vk::MemoryPropertyFlags requiredFlags, vk::MemoryPropertyFlags supportedFlags, vk::MemoryPropertyFlagBits subjectFlag) {
		return (requiredFlags & subjectFlag) != subjectFlag && (supportedFlags & subjectFlag) == subjectFlag;
	}

	unsigned selectMemoryType(std::vector<vk::MemoryPropertyFlags>& memoryPropertyPreferenceList) {
		for (auto& requiredFlags : memoryPropertyPreferenceList) {
			unsigned leastRedundancyScore = std::numeric_limits<unsigned>::max();
			unsigned mostSuitableMemoryTypeIndex = std::numeric_limits<unsigned>::max();

			for (unsigned memoryTypeIndex = 0; memoryTypeIndex < physicalDevice.memoryProperties.memoryTypeCount; memoryTypeIndex++) {
				auto& supportedFlags = physicalDevice.memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags;
				bool suitable = (supportedFlags & requiredFlags) == requiredFlags;

				if (suitable) {
					unsigned redundancyScore = 0;

					// TODO: These scores are arbitrary, change them to more meaningful values
					if (isMemoryPropertyFlagRedundant(requiredFlags, supportedFlags, vk::MemoryPropertyFlagBits::eDeviceLocal))
						redundancyScore += 1;
					if (isMemoryPropertyFlagRedundant(requiredFlags, supportedFlags, vk::MemoryPropertyFlagBits::eHostVisible))
						redundancyScore += 1;
					if (isMemoryPropertyFlagRedundant(requiredFlags, supportedFlags, vk::MemoryPropertyFlagBits::eHostCoherent))
						redundancyScore += 1;
					if (isMemoryPropertyFlagRedundant(requiredFlags, supportedFlags, vk::MemoryPropertyFlagBits::eHostCached))
						redundancyScore += 1;
					if (isMemoryPropertyFlagRedundant(requiredFlags, supportedFlags, vk::MemoryPropertyFlagBits::eLazilyAllocated))
						redundancyScore += 1;
					if (isMemoryPropertyFlagRedundant(requiredFlags, supportedFlags, vk::MemoryPropertyFlagBits::eProtected))
						redundancyScore += 1;

					if (leastRedundancyScore > redundancyScore) {
						leastRedundancyScore = redundancyScore;
						mostSuitableMemoryTypeIndex = memoryTypeIndex;
					}
				}
			}

			std::cout << "memoryTypeIndex: " << mostSuitableMemoryTypeIndex << std::endl;
			return mostSuitableMemoryTypeIndex;
		}

		// TODO: Check the value in case no memory type is suitable
		return std::numeric_limits<unsigned>::max();
	}

	Memory allocateMemory(vk::DeviceSize requiredSize, std::vector<vk::MemoryPropertyFlags> memoryPreferenceList) {
		Memory memory{
			.size = requiredSize,
			.offset = 0
		};

		auto memorTypeIndex = selectMemoryType(memoryPreferenceList);

		vk::MemoryAllocateInfo memoryInfo{
			.allocationSize = memory.size,
			.memoryTypeIndex = memorTypeIndex
		};

		memory.memory = device.allocateMemory(memoryInfo);

		auto& memoryPropertyFlags = physicalDevice.memoryProperties.memoryTypes[memorTypeIndex].propertyFlags;
		vk::MemoryPropertyFlags mappingRequirementFlags = vk::MemoryPropertyFlagBits::eHostVisible & vk::MemoryPropertyFlagBits::eHostCoherent;

		if ((memoryPropertyFlags & mappingRequirementFlags) = mappingRequirementFlags)
			memory.pointer = static_cast<unsigned char*>(device.mapMemory(memory.memory, 0, VK_WHOLE_SIZE));
		else
			memory.pointer = nullptr;

		return memory;
	}

	void createMemories() {
		const vk::DeviceSize MB = 1048576;

		vk::DeviceSize sharedMemorySize = 32 * MB;
		vk::DeviceSize deviceMemorySize = 256 * MB;

		std::vector<vk::MemoryPropertyFlags> sharedMemoryPreferenceList{
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eDeviceLocal,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		};

		std::vector<vk::MemoryPropertyFlags> deviceMemoryPreferenceList{
			vk::MemoryPropertyFlagBits::eDeviceLocal
		};

		sharedMemory = allocateMemory(sharedMemorySize, sharedMemoryPreferenceList);
		deviceMemory = allocateMemory(deviceMemorySize, deviceMemoryPreferenceList);
	}

	vk::DeviceSize alignDeviceMemoryOffset(Memory& memory, vk::MemoryRequirements requirements) {
		if (memory.offset % requirements.alignment)
			memory.offset = (memory.offset / requirements.alignment + 1) * requirements.alignment;

		vk::DeviceSize bindOffset = memory.offset;
		memory.offset += requirements.size;

		return bindOffset;
	}

	Buffer createBuffer(Memory& memory, vk::DeviceSize size, vk::BufferUsageFlags usage) {
		Buffer buffer{
			.size = size 
		};
		
		vk::BufferCreateInfo bufferInfo{
			.size = buffer.size,
			.usage = usage,
			.sharingMode = vk::SharingMode::eExclusive
		};

		buffer.buffer = device.createBuffer(bufferInfo);

		auto bufferRequirements = device.getBufferMemoryRequirements(buffer.buffer);

		buffer.offset = alignDeviceMemoryOffset(memory, bufferRequirements);

		device.bindBufferMemory(buffer.buffer, memory.memory, buffer.offset);

		if (memory.pointer)
			buffer.pointer = memory.pointer + buffer.offset;
		else
			memory.pointer = nullptr;

		return buffer;
	}

	void createBuffers() {
		vk::DeviceSize sharedBufferSize = 256;
		vk::DeviceSize deviceBufferSize = 256;

		vk::BufferUsageFlags sharedBufferUsageFlags = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eUniformBuffer;
		vk::BufferUsageFlags deviceBufferUsageFlags = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eVertexBuffer;

		sharedBuffer = createBuffer(sharedMemory, sharedBufferSize, sharedBufferUsageFlags);
		deviceBuffer = createBuffer(deviceMemory, deviceBufferSize, deviceBufferUsageFlags);
	}

	Image createImage(Memory& memory, unsigned width, unsigned height, unsigned mips, vk::SampleCountFlagBits samples, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect) {
		Image image{
			.width = width,
			.height = height
		};

		vk::ImageCreateInfo imageInfo{
			.imageType = vk::ImageType::e2D,
			.format = format,
			.extent = vk::Extent3D {
				.width = width,
				.height = height,
				.depth = 1
			},
			.mipLevels = mips,
			.arrayLayers = 1,
			.samples = samples,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = usage,
			.sharingMode = vk::SharingMode::eExclusive,
			.initialLayout = vk::ImageLayout::eUndefined
		};

		image.image = device.createImage(imageInfo);

		auto imageRequirements = device.getImageMemoryRequirements(image.image);

		auto imageOffset = alignDeviceMemoryOffset(memory, imageRequirements);
		
		device.bindImageMemory(image.image, memory.memory, imageOffset);

		vk::ImageViewCreateInfo viewInfo{
			.image = image.image,
			.viewType = vk::ImageViewType::e2D,
			.format = format,
			.components = vk::ComponentMapping {
				.r = vk::ComponentSwizzle::eIdentity,
				.g = vk::ComponentSwizzle::eIdentity,
				.b = vk::ComponentSwizzle::eIdentity,
				.a = vk::ComponentSwizzle::eIdentity
			},
			.subresourceRange = vk::ImageSubresourceRange {
				.aspectMask = aspect,
				.baseMipLevel = 0,
				.levelCount = mips,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		image.view = device.createImageView(viewInfo);

		return image;
	}

	void createObjects() {
		createMemories();
		createBuffers();
	}

	void destroyImage(Image& image) {
		device.destroyImageView(image.view);
		device.destroyImage(image.image);
	}

	void destroyObjects() {
		device.destroyBuffer(deviceBuffer.buffer);
		device.destroyBuffer(sharedBuffer.buffer);

		if (deviceMemory.pointer)
			device.unmapMemory(deviceMemory.memory);
		if (sharedMemory.pointer)
			device.unmapMemory(sharedMemory.memory);

		device.freeMemory(deviceMemory.memory);
		device.freeMemory(sharedMemory.memory);
	}
}
