#version 450
layout(binding = 1) uniform vec3 camPos;
layout(binding = 2) uniform vec3 albedo;
layout(binding = 3) uniform float metallic;
layout(binding = 4) uniform float roughness;
layout(binding = 5) uniform float ao;
layout(binding = 6) uniform vec3 lightPosition;
layout(binding = 7) uniform vec3 lightColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec2 WorldPos;
layout(location = 3) in vec2 Normal;

layout(location = 0) out vec4 outColor;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

void main() {
    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - WorldPos)

    vec3 L = normalize(lightPosition - WorldPos);
    vec3 H = normalize(V + L);

    float distance = lenght(lightPosition - WorldPos);
    float attenuation =1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation

    //Calculating Cook-Torrance specular BRDF term

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    vec3 F = fresnelSchlick(max(dot(H,V), 0.0), F0);
    

}