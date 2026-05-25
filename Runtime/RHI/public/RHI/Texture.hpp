//
// Created by cosmin on 4/28/26.
//

#pragma once

#include <glad/glad.h>
#include <Core/RefCounted.hpp>
#include <Core/intrusive_ptr.h>

namespace RHI
{
    enum class TextureDimension
    {
        Texture1D = GL_TEXTURE_1D,
        Texture2D = GL_TEXTURE_2D,
        Texture3D = GL_TEXTURE_3D,

        Texture1DArray = GL_TEXTURE_1D_ARRAY,
        Texture2DArray = GL_TEXTURE_2D_ARRAY,

        TextureCube = GL_TEXTURE_CUBE_MAP,
        TextureCubeArray = GL_TEXTURE_CUBE_MAP_ARRAY,
    };

    enum class TextureFormat
    {
        RGB8,
        RGBA8,
        Depth24Stencil8,
        RGBA16F
    };

    struct TextureDesc
    {
        uint32_t width = 0;
        uint32_t height = 0;
        TextureDimension dimension = TextureDimension::Texture2D;
        TextureFormat format = TextureFormat::RGBA8;

        //TODO: add mipmap generation, mipmap levels
        bool mipmapped = false; // replace with max mipmap level, if 0 means full chain
        bool generateMipmaps = false;

        bool renderTarget = false;
    };

    class Texture : public RefCounted
    {
        friend class Device;
        friend class CommandList;

    public:
        Texture(const TextureDesc& desc);
        ~Texture() override;

        [[nodiscard]]
        uint32_t GetWidth() const
        {
            return m_Width;
        }

        [[nodiscard]]
        uint32_t GetHeight() const
        {
            return m_Height;
        }

        bool IsMipmapped() const
        {
            return m_Mipmapped;
        }

    private:
        uint32_t m_ID = 0;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        TextureFormat m_Format;
        bool m_Mipmapped = false;
    };

    using TextureRef = core::intrusive_ptr<Texture>;
}