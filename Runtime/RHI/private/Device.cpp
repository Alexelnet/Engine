//
// Created by cosmin on 4/5/26.
//

#include <string>
#include <print>
#include <cassert>
#include <cmath>

#include <glad/glad.h>

#include <RHI/Device.hpp>
#include <Core.hpp>

#include "GLTypeConversions.hpp"

namespace RHI
{
    Device::Device(SDL_Window* window)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        m_Context = SDL_GL_CreateContext(window);

        if (!m_Context)
        {
            SDL_DestroyWindow(window);
            SDL_Quit();

            PANIC("Failed to GL context.");
        }

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
        {
            SDL_GL_DestroyContext(m_Context);
            SDL_DestroyWindow(window);
            SDL_Quit();

            PANIC("Failed to initialize GLAD.");
        }

        const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));

        std::println("OpenGL version: {}", std::string(version));

        //TODO: Extract this
        // Create the one global VAO.
        glCreateVertexArrays(1, &m_MasterVAO);
        // Bound globally. It stays bound for the lifetime of the application.
        glBindVertexArray(m_MasterVAO);
    }

    Device::~Device()
    {
        if (m_MasterVAO != 0)
            glDeleteVertexArrays(1, &m_MasterVAO);

        SDL_GL_DestroyContext(m_Context);
    }

    BufferRef Device::CreateBuffer(const void* data, uint32_t size)
    {
        return core::MakeIntrusive<Buffer>(data, size);
    }

    VertexBufferRef Device::CreateVertexBuffer(const void* data, uint32_t size)
    {
        return core::MakeIntrusive<VertexBuffer>(data, size);
    }

    IndexBufferRef Device::CreateIndexBuffer(const void* data, uint32_t size)
    {
        return core::MakeIntrusive<IndexBuffer>(data, size);
    }


    TextureRef Device::CreateTexture(const TextureDesc& desc)
    {
        return core::MakeIntrusive<Texture>(desc);
    }


    void Device::UploadTexture(TextureRef texture, const void* data, uint32_t width, uint32_t height, TextureFormat format, uint32_t mipLevel)
    {
        GLenum glFormat = RHITextureFormatToGLPixelFormat(format);
        GLenum glType = RHITextureFormatToGLType(format);

        glTextureSubImage2D(texture->m_ID, mipLevel, 0, 0, width, height, glFormat, glType, data);

        // Generate mipmaps based on the texture descriptor
        if (mipLevel == 0 && texture->IsMipmapped())
        {
            glGenerateTextureMipmap(texture->m_ID);
            int32_t levels = 0;

            // Debug
            glGetTextureParameteriv(texture->m_ID, GL_TEXTURE_MAX_LEVEL, &levels);
            std::println("Generated {} mip levels for texture", levels + 1);
        }
    }

    void Device::UploadCubemapFace(TextureRef texture, const void* data, uint32_t width, uint32_t height, TextureFormat format, uint32_t faceIndex, uint32_t mipLevel)
    {
        uint32_t glFormat = RHITextureFormatToGLPixelFormat(format);
        uint32_t glType = RHITextureFormatToGLType(format);

        glTextureSubImage3D(texture->m_ID, mipLevel, 0, 0, faceIndex, width, height, 1, glFormat, glType, data);

        if (mipLevel == 0 && texture->IsMipmapped())
            glGenerateTextureMipmap(texture->m_ID);
    }

    RenderTargetRef Device::CreateRenderTarget(const RenderTargetDesc& desc)
    {
        uint32_t frameBuffer;
        glCreateFramebuffers(1, &frameBuffer);

        // Attach color textures
        for (uint32_t i = 0; i < desc.ColorAttachments.size(); i++)
            glNamedFramebufferTexture(frameBuffer, GL_COLOR_ATTACHMENT0 + i, desc.ColorAttachments[i]->m_ID, 0);

        // Attach depth
        if (desc.DepthAttachment)
            glNamedFramebufferTexture(frameBuffer, GL_DEPTH_STENCIL_ATTACHMENT, desc.DepthAttachment->m_ID, 0);

        // Setup draw buffers
        std::vector<uint32_t> buffers;
        for (uint32_t i = 0; i < desc.ColorAttachments.size(); i++)
            buffers.push_back(GL_COLOR_ATTACHMENT0 + i);

        if (!buffers.empty())
            glNamedFramebufferDrawBuffers(frameBuffer, buffers.size(), buffers.data());

        assert(glCheckNamedFramebufferStatus(frameBuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

        auto renderTarget = core::MakeIntrusive<RenderTarget>();
        renderTarget->m_FBO = frameBuffer;
        renderTarget->m_ColorAttachments = desc.ColorAttachments;
        renderTarget->m_DepthAttachment = desc.DepthAttachment;

        return renderTarget;
    }

}
