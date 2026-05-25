//
// Created by cosmin on 4/5/26.
//

#pragma once

#include <array>
#include <cstring>
#include <type_traits>
#include <concepts>
#include <glad/glad.h>

#include <RHI/GraphicsPipeline.hpp>
#include <RHI/Buffer.hpp>
#include <RHI/VertexBuffer.hpp>
#include <RHI/RenderTarget.hpp>
#include <RHI/IndexBuffer.hpp>

#define USE_TRANSIENT_ALLOCATOR 0

#if USE_TRANSIENT_ALLOCATOR
#include "TransientAllocator.hpp"
#endif

namespace RHI
{
    class Device;
    class GraphicsPipeline;

    enum class PrimitiveTopology : uint32_t
    {
        Undefined = GL_NONE,

        PointList = GL_POINTS,
        LineList = GL_LINES,
        LineLoop = GL_LINE_LOOP,
        LineStrip = GL_LINE_STRIP,

        TriangleList = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleFan = GL_TRIANGLE_FAN,

        PatchList = GL_PATCHES
    };

    struct GeometryInfo
    {
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;
        VertexBufferRef vertexBuffer = nullptr;
        uint32_t vertexOffset = 0;
        uint32_t vertexCount = 0;
        IndexBufferRef indexBuffer = nullptr;
        uint32_t indexOffset = 0;
        uint32_t indexCount = 0;
    };

    class CommandList
    {
    public:
        explicit CommandList(Device& device);
        ~CommandList();

        CommandList& BindGraphicsPipeline(GraphicsPipelineRef graphicsPipeline);

        CommandList& Draw(const GeometryInfo& geometryInfo, uint32_t instanceCount = 1);
        CommandList& DrawFullscreenTriangle();

        // Discouraged to use directly, prefer PushConstants instead
        template<typename T>
        CommandList& SetUniform(uint32_t location, const T& value);

        CommandList& BindUniformBuffer(uint32_t location, const Buffer& buffer);

        template<typename T>
        CommandList& PushConstants(uint32_t location, const T& data);

        void BeginFrame();

        void EndFrame();

        CommandList& BeginRenderPass(RenderTargetRef target = nullptr);
        CommandList& EndRenderPass();

        CommandList& Clear(float r, float g, float b, float a);

        CommandList& BindTexture(uint32_t slot, const TextureRef& texture);

    private:
        Device& m_Device;

#if USE_TRANSIENT_ALLOCATOR
        TransientAllocator m_TransientAllocator;
        uint64_t m_FrameCounter = 0;
        std::array<GLsync, 3> m_FrameFences = { nullptr, nullptr, nullptr };
#else
        GLuint m_TransientBuffer = 0;
#endif

        // Currently bound graphics pipeline
        GraphicsPipelineRef m_CurrentGraphicsPipeline;
    };

    // Concept to detect if SetUniform has a specialization for the type T
    // TODO: Document
    template<typename T>
    concept HasSetUniformSpecialization = requires(CommandList& cmd, uint32_t loc, T value)
    {
        { cmd.SetUniform(loc, value) } -> std::same_as<CommandList&>;
    }
    && !std::is_same_v<decltype(std::declval<CommandList>().SetUniform(std::declval<uint32_t>(), std::declval<T>())), CommandList&>;

    template<typename T>
    CommandList& CommandList::PushConstants(uint32_t location, const T& data)
    {
        static_assert(sizeof(T) <= 128, "PushConstants must be at most 128 bytes.");

        if constexpr (HasSetUniformSpecialization<T>)
        {
            SetUniform(location, data);
        }
        else
        {
#if USE_TRANSIENT_ALLOCATOR
            uint32_t offset = m_TransientAllocator.Allocate(sizeof(T));
            // CPU to GPU memory copy
            std::memcpy(m_TransientAllocator.persistentPtr + offset, &data, sizeof(T));
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER, location, m_TransientAllocator.bufferID, offset, sizeof(T));

#else
            // naive implicit sync opengl
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TransientBuffer);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(T), &data);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, m_TransientBuffer);
#endif
        }

        return *this;
    }
}
