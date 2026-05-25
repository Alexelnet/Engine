#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec4 aTangent;

out vec2 vUV;
out vec3 vFragmentPosition;
out vec3 vNormal;
out vec4 vTangent;

layout(location = 0) uniform mat4 uModel;
layout(location = 1) uniform mat4 uView;
layout(location = 2) uniform mat4 uProjection;

void main()
{
    // Assigning out data
    vUV = aUV;

    vec4 worldPosition = uModel * vec4(aPosition, 1.0);
    vFragmentPosition = worldPosition.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    vNormal = normalize(normalMatrix * aNormal);

    vTangent.xyz = normalize(normalMatrix * aTangent.xyz);
    vTangent.w = aTangent.w;

    gl_Position = uProjection * uView * worldPosition;
}