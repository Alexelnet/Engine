#version 460 core

in vec2 vUV;
out vec4 FragColor;

layout(binding = 0) uniform sampler2D uHDRColor;

// ACES filmic tonemap
// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACESFilm(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;

    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 hdr = texture(uHDRColor, vUV).rgb;

    // TODO: control exposure externaly
    float exposure = 1.0;
    hdr *= exposure;

    // Apply tonemap
    vec3 tonemapped = ACESFilm(hdr);

    // Apoply gamma correction
    tonemapped = pow(tonemapped, vec3(1.0 / 2.2));

    FragColor = vec4(tonemapped, 1.0);
}