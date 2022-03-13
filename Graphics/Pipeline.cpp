module;

#include "Graphics.hpp"

module Engine:Graphics;

namespace Engine::Graphics {
	extern vk::Device device;
	extern shaderc::Compiler shaderCompiler;
	extern shaderc::CompileOptions shaderOptions;

	Pipeline::Pipeline(const char* vertexName, const char* fragmentName) {
		vertexShader = loadShader("vertex.vert");
		fragmentShader = loadShader("fragment.frag");

		createLayout();
		createPipeline();
	}

	vk::ShaderModule Pipeline::loadShader(const char* name) {
		auto path = std::filesystem::current_path() / "Shaders" / name;
		auto size = std::filesystem::file_size(path);

		shaderc_shader_kind kind;

		if (!path.extension().compare(".comp"))
			kind = shaderc_shader_kind::shaderc_glsl_compute_shader;
		else if (!path.extension().compare(".vert"))
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

		std::vector<unsigned int> data(shaderModule.cbegin(), shaderModule.cend());

		vk::ShaderModuleCreateInfo shaderInfo{
			.codeSize = static_cast<unsigned int>(data.size() * sizeof(unsigned int)),
			.pCode = data.data()
		};

		return device.createShaderModule(shaderInfo);
	}

	void Pipeline::createLayout() {
		vk::DescriptorSetLayoutBinding uniformBinding {
			.binding = 0,
			.descriptorType = vk::DescriptorType::eUniformBufferDynamic,
			.descriptorCount = 1,
			.stageFlags = vk::ShaderStageFlagBits::eVertex,
			.pImmutableSamplers = nullptr
		};

		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo {
			.bindingCount = 1,
			.pBindings = &uniformBinding
		};

		descriptorSetLayout = device.createDescriptorSetLayout(descriptorSetLayoutInfo);

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo {
			.setLayoutCount = 1,
			.pSetLayouts = &descriptorSetLayout,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr
		};

		pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);
	}

	void Pipeline::createPipeline() {

	}

	void Pipeline::destroy() {
		device.destroyPipelineLayout(pipelineLayout);
		device.destroyDescriptorSetLayout(descriptorSetLayout);

		device.destroyShaderModule(fragmentShader);
		device.destroyShaderModule(vertexShader);
	}
}