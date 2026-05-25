#version 460 core

in vec2 vUV;
out vec4 FragColor;

layout(binding = 0) uniform samplerCube uCubemap;
layout(location = 0) uniform mat4 uInvViewProj;

void main()
{
    vec4 clip = vec4(vUV * 2.0 - 1.0, 1.0, 1.0);
    vec4 worldDirection = uInvViewProj * clip;
    vec3 direction = worldDirection.xyz / worldDirection.w;

    vec3 color = texture(uCubemap, normalize(direction)).rgb;
    FragColor = vec4(color, 1.0);
}