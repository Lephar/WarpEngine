#include "internals.hpp"

namespace zero::graphics {
	uint32_t selectQueueFamily(vk::PhysicalDevice &physicalDevice, vk::SurfaceKHR &surface,
							   vk::QueueFlags queueFlags, bool presentationSupport) {
		const size_t queryCount = 5;

		uint32_t maxScore = 0;
		uint32_t mostSuitableIndex = std::numeric_limits<uint32_t>::max();

		std::array<bool, queryCount> requirementList{
				static_cast<bool>(queueFlags & vk::QueueFlagBits::eSparseBinding),
				static_cast<bool>(queueFlags & vk::QueueFlagBits::eTransfer),
				static_cast<bool>(queueFlags & vk::QueueFlagBits::eCompute),
				static_cast<bool>(queueFlags & vk::QueueFlagBits::eGraphics),
				presentationSupport
		};

		auto queueFamilies = physicalDevice.getQueueFamilyProperties();

		for (uint32_t index = 0; index < queueFamilies.size(); index++) {
			uint32_t score = 0;
			bool suitable = true;
			auto &queueFamily = queueFamilies.at(index);

			std::array<bool, queryCount> supportList{
					static_cast<bool>(queueFamily.queueFlags & vk::QueueFlagBits::eSparseBinding),
					static_cast<bool>(queueFamily.queueFlags & vk::QueueFlagBits::eTransfer),
					static_cast<bool>(queueFamily.queueFlags & vk::QueueFlagBits::eCompute),
					static_cast<bool>(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics),
					static_cast<bool>(physicalDevice.getSurfaceSupportKHR(index, surface))
			};

			for (uint32_t query = 0; query < queryCount; query++) {
				bool required = requirementList.at(query);
				bool supported = supportList.at(query);

				if (required && !supported) {
					suitable = false;
					break;
				}

				if (required == supported)
					score++;
			}

			if (!suitable)
				continue;

			if (score > maxScore) {
				maxScore = score;
				mostSuitableIndex = index;
			}
		}

		return mostSuitableIndex;
	}

	//TODO: Better physical device selection
	vk::PhysicalDevice pickPhysicalDevice(vk::Instance &instance) {
		auto physicalDevices = instance.enumeratePhysicalDevices();

		for (auto &device: physicalDevices) {
			auto properties = device.getProperties();

			if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
				return device;
		}

		return physicalDevices.front();
	}

	Internals::Internals(Instance &instance, Surface &surface) : surface(surface) {
		physicalDevice = pickPhysicalDevice(instance.get());
		transferQueueFamily = selectQueueFamily(physicalDevice, surface.get(), vk::QueueFlagBits::eTransfer, false);
		graphicsQueueFamily = selectQueueFamily(physicalDevice, surface.get(),
												vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute |
												vk::QueueFlagBits::eTransfer, true);
	}
}