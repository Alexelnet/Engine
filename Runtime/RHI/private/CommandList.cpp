//
// Created by cosmin on 4/5/26.
//

#include <print>
#include <cassert>

#include <RHI/CommandList.hpp>
#include <RHI/Device.hpp>
#include <RHI/GraphicsPipeline.hpp>

namespace RHI
{
    namespace
    {
        [[nodiscard]]
        std::tuple<int32_t, uint32_t, bool> ExtractGLAttributeInfo(VertexAttribute::Type type)
        {
            // returns { compoment count, type, is normalized }
            switch (type)
            {
                case VertexAttribute::Type::Vec2:
                    return { 2, GL_FLOAT, false };
                case VertexAttribute::Type::Vec3:
                    return { 3, GL_FLOAT, false };
                case VertexAttribute::Type::Vec4:
                    return { 4, GL_FLOAT, false };
                default:
                    assert(false && "Unknown VertexAttribute Type");
            }
        }
    }


    CommandList::CommandList(Device& device) :
        m_Device(device)
    {
#if USE_TRANSIENT_ALLOCATOR
        m_TransientAllocator.Init(8 * 1024 * 1024);
#else
        // naive implicit approach
        glCreateBuffers(1, &m_TransientBuffer);
        glNamedBufferData(m_TransientBuffer, 1024, nullptr, GL_DYNAMIC_DRAW);
#endif
    }

    CommandList::~CommandList()
    {

    }

    CommandList& CommandList::BindGraphicsPipeline(GraphicsPipelineRef graphicsPipeline)
    {
        m_CurrentGraphicsPipeline = graphicsPipeline;
        glUseProgram(graphicsPipeline->m_ShaderProgram);

        // Depth state
        glDepthMask(graphicsPipeline->m_DepthWrite == DepthWrite::Enable ? GL_TRUE : GL_FALSE);

        if (graphicsPipeline->m_DepthTest == DepthTest::Enable)
        {
            glEnable(GL_DEPTH_TEST);

            // Corresponds to RHI::DepthFunc
            static const uint32_t depthFuncs[] =
            {
                GL_ALWAYS, GL_LESS, GL_EQUAL, GL_LEQUAL,
                GL_GREATER, GL_GEQUAL, GL_NOTEQUAL
            };

            glDepthFunc(depthFuncs[static_cast<int32_t>(graphicsPipeline->m_DepthFunc)]);
        }
        else
            glDisable(GL_DEPTH_TEST);

        // Cull state
        if (graphicsPipeline->m_CullMode == CullMode::None)
            glDisable(GL_CULL_FACE);
        else
        {
            glEnable(GL_CULL_FACE);
            glCullFace(graphicsPipeline->m_CullMode == CullMode::Back ? GL_BACK : GL_FRONT);
        }

        // Front face
        glFrontFace(graphicsPipeline->m_FrontFace == FrontFace::CCW ? GL_CCW : GL_CW);

        return *this;
    }

    CommandList& CommandList::Draw(const GeometryInfo& geometryInfo, uint32_t instanceCount)
    {
        assert(m_CurrentGraphicsPipeline);

        auto& pipeline = m_CurrentGraphicsPipeline;

        // Setup vertex attributes
        for (const auto& [location, attribute] : pipeline->m_VertexAttributeList)
        {
            glEnableVertexArrayAttrib(m_Device.m_MasterVAO, location);

            const auto [count, type, normalized] = ExtractGLAttributeInfo(attribute.type);

            glVertexArrayAttribFormat(m_Device.m_MasterVAO, location, count, type, normalized, attribute.offset);
            glVertexArrayAttribBinding(m_Device.m_MasterVAO, location, 0);
        }

        glVertexArrayVertexBuffer(m_Device.m_MasterVAO, 0, geometryInfo.vertexBuffer->m_ID, 0, pipeline->m_Stride);

        if (geometryInfo.indexBuffer)
        {
            glVertexArrayElementBuffer(m_Device.m_MasterVAO, geometryInfo.indexBuffer->m_ID);

            glDrawElements(static_cast<uint32_t>(geometryInfo.topology), geometryInfo.indexCount,
                // GL_UNSIGNED_SHORT,
                GL_UNSIGNED_INT, nullptr);
        }
        else
        {
            glDrawArrays(static_cast<uint32_t>(geometryInfo.topology), geometryInfo.vertexOffset, geometryInfo.vertexCount);
        }

        return *this;
    }

    CommandList& CommandList::DrawFullscreenTriangle()
    {
        glDrawArrays(GL_TRIANGLES, 0, 3);
        return *this;
    }


    CommandList& CommandList::BindUniformBuffer(uint32_t location, const Buffer& buffer)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, location, buffer.m_ID);
        return *this;
    }

    void CommandList::BeginFrame()
    {
#if USE_TRANSIENT_ALLOCATOR
        uint32_t idx = m_FrameCounter % 3;

        // Wait for GPU if it's still using this farme's memory
        if (m_FrameFences[idx])
        {
            glClientWaitSync(m_FrameFences[idx], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
            glDeleteSync(m_FrameFences[idx]);
            m_FrameFences[idx] = nullptr;
        }

        m_TransientAllocator.Reset(idx);
#endif
    }

    void CommandList::EndFrame()
    {
#if USE_TRANSIENT_ALLOCATOR
        uint32_t idx = m_FrameCounter % 3;
        m_FrameFences[idx] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        m_FrameCounter++;
#endif
    }

    CommandList& CommandList::BeginRenderPass(RenderTargetRef target)
    {
        if (target)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, target->m_FBO);
            // Assuming all attachments are the same size, take the first one
            glViewport(0, 0, target->m_ColorAttachments[0]->GetWidth(), target->m_ColorAttachments[0]->GetHeight());
        }
        else
        {
            int32_t viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            int32_t width = viewport[2];
            int32_t height = viewport[3];

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, width, height);
        }

        return *this;
    }

    CommandList& CommandList::EndRenderPass()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return *this;
    }

    CommandList& CommandList::Clear(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return *this;
    }

    CommandList& CommandList::BindTexture(uint32_t slot, const TextureRef& texture)
    {
        glBindTextureUnit(slot, texture->m_ID);
        return *this;
    }

}

#include "CommandList.inl"
