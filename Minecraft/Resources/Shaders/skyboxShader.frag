#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec3 texCoords;

layout(binding = 0) uniform samplerCube textureSampler;

void main() {

    outColor = texture(textureSampler, texCoords);
}