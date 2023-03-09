#version 450
layout (location = 0) in vec3 aPos;

layout(location = 0) out vec3 TexCoord;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main()
{
    TexCoord = aPos;
    
    gl_Position = ubo.proj * mat4(mat3(ubo.view)) * vec4(aPos, 1.0);
}  