#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
} ubo;

layout(binding = 1) uniform StaticUBO {
    mat4 view;
    mat4 proj;
} staticUBO;

void main() 
{
    gl_Position = staticUBO.proj * ubo.model * vec4(inPosition, 1.0);
    // gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = (vec4(inColor, 1.0)).xyz;
}