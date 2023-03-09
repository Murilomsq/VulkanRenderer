#version 450 core

layout(location = 0) in vec3 TexCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform samplerCube skybox;

void main()
{    
    //outColor = vec4(1.0f);
    outColor = texture(skybox, normalize(TexCoords));
}