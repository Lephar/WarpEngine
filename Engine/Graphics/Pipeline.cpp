#include "Graphics.hpp"

namespace Engine::Graphics {
	extern PhysicalDevice physicalDevice;
	extern vk::Device device;
	
	shaderc::Compiler shaderCompiler;
	shaderc::CompileOptions shaderOptions;

	Pipeline pipeline;

	void initializePipelineDetails() {
		pipeline.mipLevels = 8;
		pipeline.maxAnisotropy = physicalDevice.properties.limits.maxSamplerAnisotropy;

		shaderOptions.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);
	}

	vk::ShaderModule loadShader(const char* name) {
		auto path = std::filesystem::current_path() / "Shaders" / name;
		auto size = std::filesystem::file_size(path);

		shaderc_shader_kind kind = shaderc_shader_kind::shaderc_glsl_compute_shader;

		if (!path.extension().compare(".vert"))
			kind = shaderc_shader_kind::shaderc_glsl_vertex_shader;
		else if (!path.extension().compare(".frag"))
			kind = shaderc_shader_kind::shaderc_glsl_fragment_shader;

		std::ifstream file(path);
		std::vector<char> code(size);
		file.read(code.data(), size);

		auto shaderModule = shaderCompiler.CompileGlslToSpv(code.data(), code.size(), kind, name, "main", shaderOptions);

#ifndef NDEBUG
		if (shaderModule.GetCompilationStatus() != shaderc_compilation_status::shaderc_compilation_status_success) {
			if (!shaderModule.GetErrorMessage().empty())
				std::cout << "SHADERC Error: " << shaderModule.GetErrorMessage() << std::endl;
			return nullptr;
		}
#endif

		std::vector<unsigned> data(shaderModule.cbegin(), shaderModule.cend());

		vk::ShaderModuleCreateInfo shaderInfo{
			.codeSize = static_cast<unsigned>(data.size() * sizeof(unsigned)),
			.pCode = data.data()
		};

		return device.createShaderModule(shaderInfo);
	}

	void createLayouts() {
		std::array<vk::DescriptorSetLayoutBinding, 2> descriptorSetLayoutBindings{
			vk::DescriptorSetLayoutBinding{
				.binding = 0,
				.descriptorType = vk::DescriptorType::eUniformBufferDynamic,
				.descriptorCount = 1,
				.stageFlags = vk::ShaderStageFlagBits::eVertex,
				.pImmutableSamplers = nullptr
			},
			vk::DescriptorSetLayoutBinding{
				.binding = 1,
				.descriptorType = vk::DescriptorType::eCombinedImageSampler,
				.descriptorCount = 1,
				.stageFlags = vk::ShaderStageFlagBits::eFragment,
				.pImmutableSamplers = nullptr
			}
		};

		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{
			.bindingCount = descriptorSetLayoutBindings.size(),
			.pBindings = descriptorSetLayoutBindings.data()
		};

		pipeline.descriptorSetLayout = device.createDescriptorSetLayout(descriptorSetLayoutInfo);

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
			.setLayoutCount = 1,
			.pSetLayouts = &pipeline.descriptorSetLayout,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr
		};

		pipeline.pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);
	}

	void createSampler() {
		vk::SamplerCreateInfo samplerInfo{
			.magFilter = vk::Filter::eLinear,
			.minFilter = vk::Filter::eLinear,
			.mipmapMode = vk::SamplerMipmapMode::eLinear,
			.addressModeU = vk::SamplerAddressMode::eRepeat,
			.addressModeV = vk::SamplerAddressMode::eRepeat,
			.addressModeW = vk::SamplerAddressMode::eRepeat,
			.mipLodBias = 0.0f,
			.anisotropyEnable = false,
			.maxAnisotropy = pipeline.maxAnisotropy,
			.compareEnable = false,
			.compareOp = vk::CompareOp::eAlways,
			.minLod = 0.0f,
			.maxLod = 1.0f,
			.borderColor = vk::BorderColor::eIntOpaqueBlack,
			.unnormalizedCoordinates = false,
		};

		pipeline.sampler = device.createSampler(samplerInfo);
	}

	void createPipeline() {
		initializePipelineDetails();

		pipeline.vertexShader = loadShader("vertex.vert");
		pipeline.fragmentShader = loadShader("fragment.frag");

		createLayouts();
		createSampler();


	}

	void destroyPipeline() {
		device.destroySampler(pipeline.sampler);

		device.destroyPipelineLayout(pipeline.pipelineLayout);
		device.destroyDescriptorSetLayout(pipeline.descriptorSetLayout);

		device.destroyShaderModule(pipeline.fragmentShader);
		device.destroyShaderModule(pipeline.vertexShader);
	}
}