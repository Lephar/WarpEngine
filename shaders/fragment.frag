#version 460 core

layout(binding = 1) uniform sampler2D textureSampler;

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec2 inputTexture;

layout(location = 0) out vec4 outputColor;

void main()
{
	vec3 ambientLight = vec3(0.2f, 0.2f, 0.2f);
	
	outputColor = vec4(ambientLight, 1.0f) * texture(textureSampler, inputTexture);
}
