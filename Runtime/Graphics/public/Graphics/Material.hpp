//
// Created by cosmin on 5/20/26.
//

#pragma once

#include <glm/glm.hpp>
#include <Graphics/Texture.hpp>

namespace Gfx
{
    struct Material
    {
        Texture albedo;
        Texture normal;
        Texture metallic;
        Texture roughness;

        glm::vec3 baseColor{1};
        float metallicFactor = 0.0f;
        float roughnessFactor = 1.0f;
    };
}