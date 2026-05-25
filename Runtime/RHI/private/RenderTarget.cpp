//
// Created by cosmin on 4/28/26.
//

#include <RHI/RenderTarget.hpp>
#include <glad/glad.h>
#include <cassert>

namespace RHI
{
    RenderTarget::~RenderTarget()
    {
        if (m_FBO)
            glDeleteFramebuffers(1, &m_FBO);
    }
}