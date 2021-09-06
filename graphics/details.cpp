#include "details.hpp"

namespace zero::graphics {
	Details::Details(Internals &internals) {
		auto &surface = internals.surface.get();
		auto &physicalDevice = internals.physicalDevice;

		auto deviceProperties = physicalDevice.getProperties();
		auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);
		auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
		auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

		auto extraImages = 1;
		auto minImages = surfaceCapabilities.minImageCount;
		auto maxImages = surfaceCapabilities.maxImageCount ? surfaceCapabilities.maxImageCount : minImages + extraImages;

		imageCount = std::min(minImages + extraImages, maxImages);
		threadCount = imageCount - minImages + 1;

		swapchainExtent = surfaceCapabilities.currentExtent;
		swapchainTransform = surfaceCapabilities.currentTransform;

		/*if(swapchainExtent.width == std::numeric_limits<uint32_t>::max() && swapchainExtent.height == std::numeric_limits<uint32_t>::max()) {
			swapchainExtent.width = window.width;
			swapchainExtent.height = window.height;
		}*/

		surfaceFormat = vk::SurfaceFormatKHR{surfaceFormats.front()};

		for (auto& format : surfaceFormats)
			if ((format.format == vk::Format::eR8G8B8A8Srgb || format.format == vk::Format::eB8G8R8A8Srgb) && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				surfaceFormat = vk::SurfaceFormatKHR{format};

		auto mailboxSupport = false;
		//auto immediateSupport = false;

		for (auto& mode : presentModes) {
			if (mode == vk::PresentModeKHR::eMailbox)
				mailboxSupport = true;
			//else if (mode == vk::PresentModeKHR::eImmediate)
			//	immediateSupport = true;
		}

		if (mailboxSupport)
			presentMode = vk::PresentModeKHR::eMailbox;
		//else if (immediateSupport)
		//	presentMode = vk::PresentModeKHR::eImmediate;
		else
			presentMode = vk::PresentModeKHR::eFifo;

		imageFormat = vk::Format::eR8G8B8A8Srgb;
		//depthStencilFormat = vk::Format::eD32SfloatS8Uint;

		mipLevels = 1;
		sampleCount = vk::SampleCountFlagBits::e2;
		maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;

		//auto alignment = deviceProperties.limits.minUniformBufferOffsetAlignment;
		//uniformAlignment = ((sizeof(glm::mat4) - 1) / alignment + 1) * alignment;

		memoryProperties = physicalDevice.getMemoryProperties();
	}
}