//
// Created by cosmin on 4/28/26.
//

#pragma once

#include <vector>
#include <Core/RefCounted.hpp>
#include <Core/intrusive_ptr.h>
#include "Texture.hpp"

namespace RHI
{
    struct RenderTargetDesc
    {
        std::vector<TextureRef> ColorAttachments;
        TextureRef DepthAttachment = nullptr;
    };

    class RenderTarget : public RefCounted
    {
        friend class Device;
        friend class CommandList;

    public:
        ~RenderTarget() override;

    private:
        uint32_t m_FBO = 0;

        std::vector<TextureRef> m_ColorAttachments;
        TextureRef m_DepthAttachment;
    };

    using RenderTargetRef = core::intrusive_ptr<RenderTarget>;
}