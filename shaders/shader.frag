#version 450

#define PI 3.14159265359


layout( binding = 1) uniform MyUniformBlock {
  vec3 camPos;
  vec3 albedo;
  float metallic;
  float roughness;
  float ao;
  vec3 lightPosition;
  vec3 lightColor;
}ubo;

layout(binding = 2) uniform samplerCube envMap;

layout(location = 0) in vec2 TexCoords;
layout(location = 1) in vec3 WorldPos;
layout(location = 2) in vec3 Normal;

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

    // finding normal and view vectors from fragment
    vec3 N = normalize(Normal);
    vec3 V = normalize(ubo.camPos - WorldPos);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, ubo.albedo, ubo.metallic);

    vec3 Lo = vec3(0.0);

    vec3 reflection = reflect(-V, N);
    vec3 envColor = texture(envMap, reflection).rgb;
    

    //Reflectance equation
    vec3 L = normalize(ubo.lightPosition - WorldPos);
    vec3 H = normalize(V + L);
    float distance = length(ubo.lightPosition - WorldPos);
    float attenuation =1.0 / (distance * distance);
    vec3 radiance = ubo.lightColor * attenuation;

   //Calculating Cook-Torrance specular part of BRDF

   float NDF = DistributionGGX(N, H, ubo.roughness);
   float G = GeometrySmith(N, V, L, ubo.roughness);
   vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
   vec3 envContrib = mix(vec3(0.0), envColor, ubo.metallic);

   vec3 kS = F;
   vec3 kD = vec3(1.0) - kS;
   kD *= 1.0 - ubo.metallic;
   
   vec3 numerator    = NDF * G * F;
   float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
   vec3 specular = numerator/denominator;

   // add to outgoing radiance L0
   float NdotL = max(dot(N,L), 0.0);
   Lo +=  (kD * ubo.albedo/ PI + specular*envColor) * radiance * NdotL;
    
   vec3 ambient = mix(vec3(0.03) * ubo.albedo * ubo.ao,envContrib, F);
   vec3 color = ambient + Lo;

   color = color / (color + vec3(1.0)); //HDR tonemapping
   color = pow(color, vec3(1.0/2.2));  // gamma correction

   outColor = vec4(color, 1.0f);

   //testing diffuse
   //outColor = vec4(ambient + ubo.albedo * max(dot(L, N),0), 1.0); 
}