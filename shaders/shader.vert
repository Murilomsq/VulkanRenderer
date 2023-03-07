#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec2 TexCoord;
layout(location = 1) out vec3 WorldPos;
layout(location = 2) out vec3 Normal;

void main() {   
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    WorldPos = vec3(ubo.model * vec4(inPosition, 1.0)); //optimize this
    TexCoord = inTexCoord;
    Normal = mat3(transpose(inverse(ubo.model))) * aNormal;
}