//
// Created by cosmin on 4/28/26.
//

#include <cmath>
#include <print>
#include <cassert>

#include <RHI/Texture.hpp>

#include "GLTypeConversions.hpp"

namespace RHI
{
    // Texture::Texture(const TextureDesc& desc) :
    //     m_Width(desc.width),
    //     m_Height(desc.height),
    //     m_Format(desc.format)
    // {
    //     glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
    //
    //     uint32_t levels = desc.mipmapped
    //         ? 1 + static_cast<uint32_t>(std::floor(std::log2(std::max(m_Width, m_Height))))
    //         : 1;
    //
    //     glTextureStorage2D(
    //         m_ID,
    //         levels,
    //         ToGLInternalFormat(desc.format),
    //         m_Width,
    //         m_Height
    //     );
    //
    //     glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //     glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //     glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //     glTextureParameteri(m_ID, GL_TEXTURE_MAX_LEVEL, levels - 1);
    // }

    Texture::Texture(const TextureDesc& desc) :
        m_Width(desc.width),
        m_Height(desc.height),
        m_Format(desc.format),
        m_Mipmapped(desc.mipmapped)
    {

        uint32_t dimension = static_cast<uint32_t>(desc.dimension);
        glCreateTextures(dimension, 1, &m_ID);

        uint32_t levels = desc.mipmapped ? 1 + static_cast<uint32_t>(std::floor(std::log2(std::max(m_Width, m_Height)))) : 1;
        glTextureStorage2D(m_ID, levels, RHITextureFormatToGLInternalFormat(desc.format), m_Width, m_Height);

        // Set filtering with mipmaps
        if (desc.mipmapped)
        {
            glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Set max level
            glTextureParameteri(m_ID, GL_TEXTURE_MAX_LEVEL, levels - 1);
        }
        else
        {
            glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_ID, GL_TEXTURE_MAX_LEVEL, 0);
        }

        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Enable anisotropic filtering
        float maxAnisotropyFactor = 0.f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropyFactor);

        glTextureParameterf(m_ID, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropyFactor);

        // Debug
        float anisotropyFactor;
        glGetTextureParameterfv(m_ID, GL_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropyFactor);

        std::println("Anisotropy = {}", anisotropyFactor);
    }


    Texture::~Texture()
    {
        if (m_ID)
            glDeleteTextures(1, &m_ID);
    }
}