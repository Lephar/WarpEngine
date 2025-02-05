#include "shader.h"

#include "helper.h"
#include "file.h"
#include "buffer.h"
#include "content.h"

shaderc_compiler_t shaderCompiler;
shaderc_compile_options_t shaderCompileOptions;

extern VkDevice device;

extern VkDescriptorSetLayout descriptorSetLayout;

ShaderModule   vertexShaderModule;
ShaderModule fragmentShaderModule;

ShaderCode loadShaderCode(const char *file, FileType type, shaderc_shader_kind stage) {
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "shaders/%s", file);

    ShaderCode shaderCode = {
        .type = type,
        .stage = stage,
        .data = readFile(path, type)
    };

    debug("\tFile loading successful");

    return shaderCode;
}

void compileShaderCode(ShaderCode *shaderCode) {
    assert(shaderCode->type == FILE_TYPE_TEXT);

    shaderc_compilation_result_t result = shaderc_compile_into_spv(shaderCompiler, shaderCode->data.string, shaderCode->data.size - 1, shaderCode->stage, "shader", "main", shaderCompileOptions);
    shaderc_compilation_status status = shaderc_result_get_compilation_status(result);

    if(status != shaderc_compilation_status_success) {
        debug("%s", shaderc_result_get_error_message(result));
        shaderc_result_release(result);
        assert(status == shaderc_compilation_status_success);
    }

    shaderCode->type = FILE_TYPE_BINARY,
    freeData(&shaderCode->data);
    shaderCode->data = makeData(shaderc_result_get_length(result), shaderc_result_get_bytes(result));
    shaderc_result_release(result);

    debug("\tCode compilation successful");
}

void freeShaderCode(ShaderCode *shaderCode) {
    freeData(&shaderCode->data);
}

ShaderModule createShaderModule(ShaderCode shaderCode) {
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
    debug("\tModule creation successful");

    return shaderModule;
}

void destroyShaderModule(ShaderModule *shaderModule) {
    PFN_vkDestroyShaderEXT destroyShader = loadFunction("vkDestroyShaderEXT");
    assert(destroyShader);
    destroyShader(device, shaderModule->module, NULL);
}

ShaderModule makeShaderModule(const char *file, FileType type, shaderc_shader_kind stage) {
    debug("Loading and creating shader module %s", file);

    ShaderCode shaderCode = loadShaderCode(file, type, stage);

    if(type == FILE_TYPE_TEXT) {
        compileShaderCode(&shaderCode);
    }

    ShaderModule shaderModule = createShaderModule(shaderCode);
    freeShaderCode(&shaderCode);

    return shaderModule;
}

void createModules() {
    shaderCompiler = shaderc_compiler_initialize();

    shaderCompileOptions = shaderc_compile_options_initialize();
#ifndef DEBUG
    shaderc_compile_options_set_optimization_level(shaderCompileOptions, shaderc_optimization_level_performance);
#endif // DEBUG

    debug("Shader compiler and shader compile options set");

    vertexShaderModule   = makeShaderModule("vertex_fixed.vert", FILE_TYPE_TEXT, shaderc_vertex_shader);
    fragmentShaderModule = makeShaderModule("fragment_fixed.frag.spv", FILE_TYPE_BINARY, shaderc_fragment_shader);

    debug("Shader modules created");
}

void destroyModules() {
    shaderc_compiler_release(shaderCompiler);
    shaderc_compile_options_release(shaderCompileOptions);

    debug("Shader compiler and shader compile options released");

    destroyShaderModule(&vertexShaderModule);
    destroyShaderModule(&fragmentShaderModule);

    debug("Shader modules destroyed");
}
