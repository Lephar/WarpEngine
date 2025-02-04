#include "shader.h"

#include "helper.h"
#include "file.h"
#include "buffer.h"
#include "content.h"

shaderc_compiler_t shaderCompiler;
shaderc_compile_options_t shaderCompileOptions;

extern VkDevice device;

extern VkDescriptorSetLayout descriptorSetLayout;

ShaderCode   vertexShaderCode;
ShaderCode fragmentShaderCode;

ShaderModule   vertexShaderModule;
ShaderModule fragmentShaderModule;

ShaderCode loadShaderCode(const char *path, FileType type, shaderc_shader_kind stage) {
    ShaderCode shaderCode = {
        .type = type,
        .stage = stage,
        .data = readFile(path, type)
    };

    return shaderCode;
}

ShaderCode compileShaderCode(ShaderCode shaderCode) {
    assert(shaderCode.type == FILE_TYPE_TEXT);

    shaderc_compilation_result_t result = shaderc_compile_into_spv(shaderCompiler, shaderCode.data.content, shaderCode.data.size - 1, shaderCode.stage, "shader", "main", shaderCompileOptions);
    shaderc_compilation_status status = shaderc_result_get_compilation_status(result);

    if(status != shaderc_compilation_status_success) {
        debug("%s", shaderc_result_get_error_message(result));
        shaderc_result_release(result);
        assert(status == shaderc_compilation_status_success);
    }

    debug("\tSuccessfully compiled");

    ShaderCode compiledShaderCode = {
        .type = FILE_TYPE_BINARY,
        .stage = shaderCode.stage,
        .data = makeData(shaderc_result_get_length(result), shaderc_result_get_bytes(result))
    };

    debug("\tCompiled size: %ld", compiledShaderCode.data.size);

    shaderc_result_release(result);
}

ShaderModule createModule(ShaderCode shaderCode) {
    ShaderModule shaderModule = {};

    if(shaderCode.stage == shaderc_compute_shader) {
        shaderModule.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    } else if(shaderCode.stage == shaderc_vertex_shader) {
        shaderModule.stage     = VK_SHADER_STAGE_VERTEX_BIT;
        shaderModule.nextStage = VK_SHADER_STAGE_FRAGMENT_BIT;
    } else if(shaderCode.stage == shaderc_fragment_shader) {
        shaderModule.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    } //TODO: Add other shader types

    VkShaderCreateInfoEXT shaderCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT,
        .pNext = NULL,
        .flags = VK_SHADER_CREATE_LINK_STAGE_BIT_EXT,
        .stage = shaderModule.stage,
        .nextStage = shaderModule.nextStage,
        .codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT,
        .codeSize = shaderCode.data.size,
        .pCode = shaderCode.data.content,
        .pName = "main",
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
        .pSpecializationInfo = NULL
    };

    PFN_vkCreateShadersEXT createShaders = loadFunction("vkCreateShadersEXT");
    assert(createShaders);

    createShaders(device, 1, &shaderCreateInfo, NULL, &shaderModule.module);
    debug("\tSuccessfully created");

    return shaderModule;
}

void createModules() {
    shaderCompiler = shaderc_compiler_initialize();

    shaderCompileOptions = shaderc_compile_options_initialize();
#ifdef DEBUG
    shaderc_compile_options_set_optimization_level(shaderCompileOptions, shaderc_optimization_level_performance);
#endif // DEBUG

    debug("Shader compiler and shader compile options set");

    vertexShader = createModule(vertexShaderCode);

    debug("Shader modules created");
}

void destroyModule(Shader *shader) {
    PFN_vkDestroyShaderEXT destroyShader = loadFunction("vkDestroyShaderEXT");
    assert(destroyShader);
    destroyShader(device, shader->module, NULL);

    free(shader->data);

    debug("Shader module %s destroyed", shader->name);
}

void destroyModules() {
    shaderc_compiler_release(shaderCompiler);
    shaderc_compile_options_release(shaderCompileOptions);

    debug("Shader compiler and shader compile options released");

    for(uint32_t shaderIndex = 0; shaderIndex < shaderCount; shaderIndex++) {
        destroyModule(shaderReferences[shaderIndex]);
    }

    debug("Shader modules destroyed");
}
