//
// Created by cosmin on 5/20/26.
//

#pragma once

#include <RHI/Texture.hpp>
#include <glm/glm.hpp>

namespace Gfx
{
    enum class TextureUsage
    {
        // RGBA, default = white
        Color,
        // RGB to RGBA, default = flat normal
        Normal,
        // grayscale to RGBA, default = black
        Mask
    };

    struct Texture
    {
        RHI::TextureRef handle;

        Texture() = default;

        explicit Texture(RHI::TextureRef h) :
            handle(std::move(h))
        {

        }

        static void InitializeDefaults(RHI::Device& device);

        static const Texture& White();
        static const Texture& Black();
        static const Texture& Gray();
        static const Texture& FlatNormal();

        static Texture Create(RHI::Device& device, const glm::vec4& color);

        static Texture Load(RHI::Device& device, const std::string& path, TextureUsage usage);

    private:
        static Texture CreateInternal(RHI::Device& device, const void* data, uint32_t width, uint32_t height);
    };
}