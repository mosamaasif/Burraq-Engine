#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;

layout (push_constant) uniform constants {

    mat4 u_VP;
    
} PushConstants;

void main() {
    gl_Position = PushConstants.u_VP * vec4(inPosition, 1.0f);
    fragColor = inNormal * 0.5f + vec3(0.5f);
}