#include "Graphics.hpp"

#include <SDL_log.h>
#include <fstream>
#include <filesystem>

#ifndef NDEBUG
SDL_LogPriority convertLogSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
	if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		return SDL_LOG_PRIORITY_ERROR;
	else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		return SDL_LOG_PRIORITY_WARN;
	else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		return SDL_LOG_PRIORITY_INFO;
	else if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		return SDL_LOG_PRIORITY_VERBOSE;
	else
		return SDL_LOG_PRIORITY_DEBUG;
}

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	static_cast<void>(type);
	static_cast<void>(pUserData);

	SDL_LogMessage(SDL_LOG_CATEGORY_RENDER, convertLogSeverity(severity), "\tValidation Layers:\t%s\n", pCallbackData->pMessage);

	return VK_FALSE;
}
#endif // NDEBUG
/*
void Graphics::loadAsset(std::string fileName) {
	auto filePath = std::filesystem::current_path() / "assets" / fileName;
	std::ifstream asset(filePath.c_str());

	Index offset = vertices.size();

	std::string type;

	while(asset >> type) {
		if(!type.compare("v")) {
			float x, y, z, r, g, b;
			asset >> x >> y >> z;

			r = static_cast<float>(rand()) / RAND_MAX;
			g = static_cast<float>(rand()) / RAND_MAX;
			b = static_cast<float>(rand()) / RAND_MAX;

			Vertex vertex{glm::vec3{x, y, z}, glm::vec3{r, g, b}};
			vertices.push_back(vertex);
		}

		else if(!type.compare("f")) {
			Index a, b, c;
			asset >> a >> b >> c;

			a += offset - 1;
			b += offset - 1;
			c += offset - 1;

			indices.insert(indices.end(), {a, b, c});
		}
	}
}

vk::ShaderModule Graphics::loadShader(std::string fileName, shaderc_shader_kind shaderKind) {
	auto filePath = std::filesystem::current_path() / "shaders" / fileName;
	auto codeSize = std::filesystem::file_size(filePath);

	std::ifstream shaderFile(filePath.c_str());
	std::vector<char> shaderCode(codeSize);
	shaderFile.read(shaderCode.data(), codeSize);

	auto shaderModule = shaderCompiler.CompileGlslToSpv(shaderCode.data(), shaderCode.size(), shaderKind, fileName.c_str(), "main", shaderOptions);

	if(shaderModule.GetCompilationStatus() != shaderc_compilation_status::shaderc_compilation_status_success) {
		auto message = shaderModule.GetErrorMessage();

		if(!message.empty())
			std::cout << message << std::endl;

		return nullptr;
	}

	std::vector<unsigned int> shaderBinary(shaderModule.cbegin(), shaderModule.cend());
	vk::ShaderModuleCreateInfo shaderModuleInfo{};
	shaderModuleInfo.codeSize = shaderBinary.size() * sizeof(unsigned int);
	shaderModuleInfo.pCode = shaderBinary.data();

	return device.createShaderModule(shaderModuleInfo);
}
*/
vk::PhysicalDevice Graphics::pickPhysicalDevice() {
	auto physicalDevices = instance.enumeratePhysicalDevices();

	for (auto& physicalDeviceCandidate : physicalDevices)
		if (physicalDeviceCandidate.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			return physicalDeviceCandidate;

	return physicalDevices.front();
}

uint32_t Graphics::selectQueueFamily() {
	auto queueFamilies = physicalDevice.getQueueFamilyProperties();

	for(uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilies.size(); queueFamilyIndex++) {
		auto& queueFamily = queueFamilies.at(queueFamilyIndex);

		if(physicalDevice.getSurfaceSupportKHR(queueFamilyIndex, surface) &&
			(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) &&
			(queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)) {
			return queueFamilyIndex;
		}
	}

	return std::numeric_limits<uint32_t>::max();
}

vk::SurfaceFormatKHR Graphics::selectSurfaceFormat() {
	std::vector<vk::SurfaceFormatKHR> preferredSurfaceFormats {
		vk::SurfaceFormatKHR {
			vk::Format::eB8G8R8A8Srgb,
			vk::ColorSpaceKHR::eSrgbNonlinear
		}
	};

	auto supportedSurfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);

	for(auto preferredSurfaceFormat : preferredSurfaceFormats)
		for(auto supportedSurfaceFormat : supportedSurfaceFormats)
			if(preferredSurfaceFormat.format == supportedSurfaceFormat.format && preferredSurfaceFormat.colorSpace == supportedSurfaceFormat.colorSpace)
				return preferredSurfaceFormat;

	return supportedSurfaceFormats.front();
}

vk::PresentModeKHR Graphics::selectPresentMode() {
	std::vector<vk::PresentModeKHR> preferredPresentModes {
		vk::PresentModeKHR::eMailbox,
		vk::PresentModeKHR::eImmediate,
		vk::PresentModeKHR::eFifoRelaxed,
		vk::PresentModeKHR::eFifo
	};

	auto supportedPresentModes = physicalDevice.getSurfacePresentModesKHR(surface);

	for(auto& preferredPresentMode : preferredPresentModes)
		for(auto& supportedPresentMode : supportedPresentModes)
			if(preferredPresentMode == supportedPresentMode)
				return preferredPresentMode;

	return supportedPresentModes.front();
}

vk::CommandBuffer Graphics::beginSingleTimeCommand() {
	vk::CommandBufferAllocateInfo allocateInfo {
		.commandPool = commandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};

	auto commandBuffer = device.allocateCommandBuffers(allocateInfo).front();

	vk::CommandBufferBeginInfo beginInfo {
		.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
	};

	commandBuffer.begin(beginInfo);

	return commandBuffer;
}

void Graphics::endSingleTimeCommand(vk::CommandBuffer commandBuffer) {
	vk::SubmitInfo submitInfo{
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer
	};

	commandBuffer.end();
	queue.submit(submitInfo);
    queue.waitIdle();
}

