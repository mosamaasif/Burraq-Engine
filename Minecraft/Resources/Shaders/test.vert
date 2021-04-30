#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outTexCoords;

layout (push_constant) uniform constants {

    mat4 u_VP;
    
} PushConstants;

layout(binding = 0) uniform UniformBufferObject {
    float model;
} ubo;

void main() {
    
    gl_Position = PushConstants.u_VP * vec4(inPosition, 1.0f) * ubo.model;

    outTexCoords = inPosition;
    outTexCoords.xy *= -1.0f;
}