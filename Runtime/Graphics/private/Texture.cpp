//
// Created by cosmin on 5/20/26.
//

#include <filesystem>

#include <Core.hpp>
#include <RHI/Device.hpp>
#include <Graphics/Texture.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb/stb_image.h>

namespace Gfx
{
    // Global default fallback textures
    static inline Texture s_White;
    static inline Texture s_Black;
    static inline Texture s_Gray;
    static inline Texture s_FlatNormal;

    Texture Texture::CreateInternal(RHI::Device& device, const void* data, uint32_t width, uint32_t height)
    {
        RHI::TextureDesc desc
        {
            .width = width,
            .height = height,
            .format = RHI::TextureFormat::RGBA8,
            .mipmapped = true,
            .generateMipmaps = true,
        };

        Texture tex(device.CreateTexture(desc));

        device.UploadTexture(tex.handle, data, width, height, RHI::TextureFormat::RGBA8);

        return tex;
    }

    Texture Texture::Create(RHI::Device& device, const glm::vec4& color)
    {
        uint8_t pixel[4] =
        {
            static_cast<uint8_t>(color.r * 255.0f),
            static_cast<uint8_t>(color.g * 255.0f),
            static_cast<uint8_t>(color.b * 255.0f),
            static_cast<uint8_t>(color.a * 255.0f)
        };

        return CreateInternal(device, pixel, 1, 1);
    }

    void Texture::InitializeDefaults(RHI::Device& device)
    {
        s_White = Create(device, {1, 1, 1, 1});
        s_Black = Create(device, {0, 0, 0, 1});
        s_Gray = Create(device, {0.5f, 0.5f, 0.5f, 1});
        s_FlatNormal = Create(device, {0.5f, 0.5f, 1.0f, 1});
    }

    const Texture& Texture::White()
    {
        return s_White;
    }

    const Texture& Texture::Black()
    {
        return s_Black;
    }

    const Texture& Texture::Gray()
    {
        return s_Gray;
    }

    const Texture& Texture::FlatNormal()
    {
        return s_FlatNormal;
    }

    Texture Texture::Load(RHI::Device& device, const std::string& path, TextureUsage usage)
    {
        if (!std::filesystem::exists(path))
        {
            switch (usage)
            {
                case TextureUsage::Color:
                    return White();

                case TextureUsage::Normal:
                    return FlatNormal();

                case TextureUsage::Mask:
                    return Black();
            }
        }

        int32_t width, height, channels;

        int32_t requestedChannels = usage == TextureUsage::Mask ? 1 : usage == TextureUsage::Normal ? 3 : 4;

        unsigned char* src = stbi_load(path.c_str(), &width, &height, &channels, requestedChannels);

        if (!src)
            PANIC("Failed to load texture: {}", path);

        if (usage == TextureUsage::Color)
        {
            auto tex = CreateInternal(device, src, width, height);

            stbi_image_free(src);
            return tex;
        }

        std::vector<uint8_t> rgba(width * height * 4);

        if (usage == TextureUsage::Mask)
        {
            for (uint32_t i = 0; i < width * height; i++)
            {
                uint8_t v = src[i];

                rgba[i * 4 + 0] = v;
                rgba[i * 4 + 1] = v;
                rgba[i * 4 + 2] = v;
                rgba[i * 4 + 3] = 255;
            }
        }
        else if (usage == TextureUsage::Normal)
        {
            for (uint32_t i = 0; i < width * height; i++)
            {
                rgba[i * 4 + 0] = src[i * 3 + 0];
                rgba[i * 4 + 1] = src[i * 3 + 1];
                rgba[i * 4 + 2] = src[i * 3 + 2];
                rgba[i * 4 + 3] = 255;
            }
        }

        stbi_image_free(src);

        return CreateInternal(device, rgba.data(), width, height);
    }
}