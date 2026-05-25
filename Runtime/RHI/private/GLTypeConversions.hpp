//
// Created by cosmin on 5/20/26.
//

#pragma once

#include <string>
#include <cassert>

#include <glad/glad.h>

#include <RHI/Device.hpp>

namespace RHI
{
    static GLenum RHITextureFormatToGLInternalFormat(TextureFormat format)
    {
        switch (format)
        {
            case TextureFormat::RGB8:
                return GL_RGB8;
            case TextureFormat::RGBA8:
                return GL_RGBA8;
            case TextureFormat::RGBA16F:
                return GL_RGBA16F;
            case TextureFormat::Depth24Stencil8:
                return GL_DEPTH24_STENCIL8;
        }

        assert(false);
        return GL_RGBA8;
    }

    static GLenum RHITextureFormatToGLPixelFormat(TextureFormat format)
    {
        switch (format)
        {
            case TextureFormat::RGB8:
                return GL_RGB;
            case TextureFormat::RGBA8:
            case TextureFormat::RGBA16F:
                return GL_RGBA;
            case TextureFormat::Depth24Stencil8:
                return GL_DEPTH_STENCIL;
        }

        assert(false);
        return GL_RGBA;
    }

    static GLenum RHITextureFormatToGLType(TextureFormat format)
    {
        switch (format)
        {
            case TextureFormat::RGB8:
            case TextureFormat::RGBA8:
                return GL_UNSIGNED_BYTE;
            case TextureFormat::RGBA16F:
                return GL_FLOAT;
            case TextureFormat::Depth24Stencil8:
                return GL_UNSIGNED_INT_24_8;
        }

        assert(false);
        return GL_UNSIGNED_BYTE;
    }
}