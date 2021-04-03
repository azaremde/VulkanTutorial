#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inColor;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoords;

layout(binding = 0) uniform DynamicUBO {
    mat4 model;
} dynamicUBO;

layout(binding = 1) uniform StaticUBO {
    mat4 view;
    mat4 proj;
} staticUBO;

void main() 
{
    gl_Position = staticUBO.proj * dynamicUBO.model * vec4(inPosition, 1.0);
    fragColor = vec3(inColor, 1.0);

    fragTexCoords = inColor;
}