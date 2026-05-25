//
// Created by cosmin on 4/5/26.
//

#pragma once

#include <SDL3/SDL.h>

#include <RHI/Buffer.hpp>
#include <RHI/VertexBuffer.hpp>
#include <RHI/Texture.hpp>
#include <RHI/RenderTarget.hpp>
#include <RHI/IndexBuffer.hpp>

namespace RHI
{
    class Device
    {
        friend class CommandList;

    public:
        Device(SDL_Window* window);
        ~Device();

        BufferRef CreateBuffer(const void* data, uint32_t size);
        VertexBufferRef CreateVertexBuffer(const void* data, uint32_t size);
        IndexBufferRef CreateIndexBuffer(const void* data, uint32_t size);

        TextureRef CreateTexture(const TextureDesc& desc);
        RenderTargetRef CreateRenderTarget(const RenderTargetDesc& desc);

        void UploadTexture(TextureRef texture, const void* data, uint32_t width, uint32_t height, TextureFormat format, uint32_t mipLevel = 0);
        void UploadCubemapFace(TextureRef texture, const void* data, uint32_t width, uint32_t height, TextureFormat format, uint32_t faceIndex, uint32_t mipLevel = 0);

    private:

        SDL_GLContext m_Context {};
        uint32_t m_MasterVAO = 0;
    };
}