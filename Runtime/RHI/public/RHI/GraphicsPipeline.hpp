//
// Created by cosmin on 4/6/26.
//

#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>

#include <glad/glad.h>

#include <RHI/VertexAttributeList.hpp>

#include <Core/intrusive_ptr.h>
#include "Core/RefCounted.hpp"

namespace RHI
{
    class Device;

    enum class ShaderType
    {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Compute = GL_COMPUTE_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
        TessellationControl = GL_TESS_CONTROL_SHADER,
        TessellationEvaluation = GL_TESS_EVALUATION_SHADER,
    };

    enum class CullMode
    {
        None,
        Front,
        Back
    };

    enum class FrontFace
    {
        CCW,
        CW
    };

    enum class DepthWrite
    {
        Disable,
        Enable
    };

    enum class DepthTest
    {
        Disable,
        Enable
    };

    enum class DepthFunc
    {
        Always,
        Less,
        Equal,
        Lequal,
        Greater,
        Gequal,
        NotEqual
    };

    class GraphicsPipeline;
    using GraphicsPipelineRef = core::intrusive_ptr<GraphicsPipeline>;

    class GraphicsPipeline : public RefCounted
    {
        friend class CommandList;

    public:

        GraphicsPipeline() = default;
        ~GraphicsPipeline() override;

        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

        class Builder
        {
        public:
            Builder& SetInputAssembly(const VertexAttributeList& vertexAttributeList);

            Builder& AddShader(ShaderType type, std::string_view code);

            Builder& SetCullMode(CullMode mode)
            {
                m_CullMode = mode;
                return *this;
            }

            Builder& SetFrontFace(FrontFace face)
            {
                m_FrontFace = face;
                return *this;
            }

            Builder& SetDepthWrite(DepthWrite value)
            {
                m_DepthWrite = value; return *this;
            }

            Builder& SetDepthTest(DepthTest value)
            {
                m_DepthTest  = value; return *this;
            }

            Builder& SetDepthFunc(DepthFunc value)
            {
                m_DepthFunc  = value; return *this;
            }

            [[nodiscard]]
            GraphicsPipelineRef Build(Device& device);

        private:
            VertexAttributeList m_VertexAttributeList;

            std::unordered_map<ShaderType, std::string_view> m_ShaderSources;
            std::unordered_map<ShaderType, uint32_t> m_Shaders;

            uint32_t m_ShaderProgram = 0;

            CullMode m_CullMode = CullMode::Back;
            FrontFace m_FrontFace = FrontFace::CCW;

            DepthWrite m_DepthWrite = DepthWrite::Enable;
            DepthTest  m_DepthTest  = DepthTest::Enable;
            DepthFunc  m_DepthFunc  = DepthFunc::Less;
        };

    private:
        VertexAttributeList m_VertexAttributeList;

        // TODO: temp?
        uint32_t m_Stride = 0;

        uint32_t m_ShaderProgram = 0;

        CullMode m_CullMode = CullMode::Back;
        FrontFace m_FrontFace = FrontFace::CCW;

        DepthWrite m_DepthWrite = DepthWrite::Enable;
        DepthTest  m_DepthTest  = DepthTest::Enable;
        DepthFunc  m_DepthFunc  = DepthFunc::Less;
    };
}
