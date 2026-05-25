#version 460 core

in vec2 vUV;
in vec3 vFragmentPosition;
in vec3 vNormal;
in vec4 vTangent;

out vec4 FragColor;

// Camera inputs
layout(location = 3) uniform vec3 uCameraPos;

// Light inputs
layout(location = 4) uniform vec3 uLightDir;
layout(location = 5) uniform vec3 uLightColor;
layout(location = 6) uniform float uLightIntensity;

// Material inputs
layout(binding = 0) uniform sampler2D uAlbedoMap;
layout(binding = 1) uniform sampler2D uMetallicMap;
layout(binding = 2) uniform sampler2D uRoughnessMap;
layout(binding = 3) uniform sampler2D uNormalMap;

uniform float uMetallicFactor = 1.0;
uniform float uRoughnessFactor = 1.0;

const float PI = 3.14159265359;

// Sources
// https://learnopengl.com/PBR/Theory
// https://learnopengl.com/PBR/Lighting
// https://docs.vulkan.org/tutorial/latest/Building_a_Simple_Engine/Lighting_Materials/04_lighting_implementation.html

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = max(roughness, 0.04); // To avoid having 0 roughtness
    float a2 = a * a;
    a2 *= a2;

    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return a2 / max(denom, 0.0001);
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 GetNormalFromMap()
{
    // Sample normal map
    vec3 tangentNormal = texture(uNormalMap, vUV).rgb;
    // Convert from [0,1] → [-1,1] because 0.5 corresponds to 0
    tangentNormal = tangentNormal * 2.0 - 1.0;

    // Normals
    vec3 N = normalize(vNormal);
    // Tangents
    vec3 T = normalize(vTangent.xyz - N * dot(N, vTangent.xyz));
    // Bitangents
    vec3 B = cross(N, T) * vTangent.w;

    // Tangent space to world space
    mat3 TBN = mat3(T, B, N);

    // Convert into into world spce
    return normalize(TBN * tangentNormal);
}

void main()
{
    // Material inputs
    vec3 albedo = texture(uAlbedoMap, vUV).rgb;
    float metallic = texture(uMetallicMap, vUV).r * uMetallicFactor;
    float roughness = texture(uRoughnessMap, vUV).r * uRoughnessFactor;
    roughness = clamp(roughness, 0.04, 1.0);

    // Geometry vectors

    // Normal
    vec3 N = GetNormalFromMap();
    // View direction
    vec3 V = normalize(uCameraPos - vFragmentPosition);
    // Light direction
    vec3 L = normalize(-uLightDir);
    // Half vector for muicrofacets
    vec3 H = normalize(V + L);

    vec3 radiance = uLightColor * uLightIntensity;

    // Fresnel
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    // Split diffuse and specular
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    float NdotL = max(dot(N, L), 0.0);

    // Final light contributuon
    vec3 final = (kD * albedo / PI + specular) * radiance * NdotL;

    // Hardcoded
    vec3 ambient = vec3(0.03) * albedo;

    FragColor = vec4(ambient + final, 1.0);
}