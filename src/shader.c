#include "shader.h"

#include "device.h"
#include "pipeline.h"

#include "data.h"
#include "file.h"
#include "logger.h"

shaderc_compiler_t        shaderCompiler;
shaderc_compile_options_t shaderCompileOptions;

ShaderModule   skyboxShaderModule;
ShaderModule   vertexShaderModule;
ShaderModule fragmentShaderModule;

ShaderCode loadShaderCode(const char *path, bool binary, shaderc_shader_kind stage) {
    char fullPath[PATH_MAX];
    makeFullPath("shaders", path, fullPath);

    ShaderCode shaderCode = {
        .stage = stage,
        .data = readFile(binary, fullPath)
    };

    debug("\tFile loading successful");

    return shaderCode;
}

void compileShaderCode(ShaderCode *shaderCode) {
    assert(!shaderCode->data->binary);

    shaderc_compilation_result_t result = shaderc_compile_into_spv(shaderCompiler, shaderCode->data->content, shaderCode->data->size - 1, shaderCode->stage, "shader", "main", shaderCompileOptions);
    shaderc_compilation_status status = shaderc_result_get_compilation_status(result);

    if(status != shaderc_compilation_status_success) {
        debug("%s", shaderc_result_get_error_message(result));
        shaderc_result_release(result);
        assert(status == shaderc_compilation_status_success);
    }

    freeData(shaderCode->data);
    shaderCode->data = makeData(true, shaderc_result_get_length(result), shaderc_result_get_bytes(result));
    shaderc_result_release(result);

    debug("\tCode compilation successful");
}

void freeShaderCode(ShaderCode *shaderCode) {
    freeData(shaderCode->data);
}

ShaderModule createShaderModule(ShaderCode shaderCode) {
    ShaderModule shaderModule = {};

    if( shaderCode.stage       == shaderc_compute_shader) {
        shaderModule.stage     =  VK_SHADER_STAGE_COMPUTE_BIT;
    } else if(shaderCode.stage == shaderc_vertex_shader) {
        shaderModule.stage     =  VK_SHADER_STAGE_VERTEX_BIT;
        shaderModule.nextStage =  VK_SHADER_STAGE_FRAGMENT_BIT;
    } else if(shaderCode.stage == shaderc_fragment_shader) {
        shaderModule.stage     =  VK_SHADER_STAGE_FRAGMENT_BIT;
    } // TODO: Add other shader types

    VkShaderCreateInfoEXT shaderCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT,
        .pNext = NULL,
        .flags = VK_SHADER_CREATE_LINK_STAGE_BIT_EXT,
        .stage = shaderModule.stage,
        .nextStage = shaderModule.nextStage,
        .codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT,
        .codeSize = shaderCode.data->size,
        .pCode = shaderCode.data->content,
        .pName = "main",
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
        .pSpecializationInfo = NULL
    };

    PFN_vkCreateShadersEXT createShaders = loadDeviceFunction("vkCreateShadersEXT");
    createShaders(device, 1, &shaderCreateInfo, NULL, &shaderModule.module);
    debug("\tModule creation successful");

    return shaderModule;
}

void destroyShaderModule(ShaderModule *shaderModule) {
    PFN_vkDestroyShaderEXT destroyShader = loadDeviceFunction("vkDestroyShaderEXT");
    destroyShader(device, shaderModule->module, NULL);
}

ShaderModule makeShaderModule(const char *file, bool binary, shaderc_shader_kind stage) {
    debug("Loading and creating shader module %s", file);

    ShaderCode shaderCode = loadShaderCode(file, binary, stage);

    if(!binary) {
        compileShaderCode(&shaderCode);
    }

    ShaderModule shaderModule = createShaderModule(shaderCode);
    freeShaderCode(&shaderCode);

    return shaderModule;
}

void createModules() {
    shaderCompiler = shaderc_compiler_initialize();
    assert(shaderCompiler);

    shaderCompileOptions = shaderc_compile_options_initialize();
    assert(shaderCompileOptions);

    shaderc_compile_options_set_source_language(shaderCompileOptions, shaderc_source_language_glsl);
    shaderc_compile_options_set_target_env(shaderCompileOptions, shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
    shaderc_compile_options_set_target_spirv(shaderCompileOptions, shaderc_spirv_version_1_6);
#if DEBUG
    shaderc_compile_options_set_generate_debug_info(shaderCompileOptions);
#else
    shaderc_compile_options_set_optimization_level(shaderCompileOptions, shaderc_optimization_level_performance);
#endif

    debug("Shader compiler created and compile options set");

    skyboxShaderModule   = makeShaderModule("skybox.vert",       false,  shaderc_vertex_shader);
    vertexShaderModule   = makeShaderModule("vertex.vert",       false,  shaderc_vertex_shader);
    fragmentShaderModule = makeShaderModule("fragment.frag.spv", true, shaderc_fragment_shader);

    debug("Shader modules created");
}

void destroyModules() {
    shaderc_compiler_release(shaderCompiler);
    shaderc_compile_options_release(shaderCompileOptions);

    debug("Shader compiler and shader compile options released");

    destroyShaderModule(&skyboxShaderModule);
    destroyShaderModule(&vertexShaderModule);
    destroyShaderModule(&fragmentShaderModule);

    debug("Shader modules destroyed");
}
