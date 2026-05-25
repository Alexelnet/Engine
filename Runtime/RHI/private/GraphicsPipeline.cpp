//
// Created by cosmin on 4/6/26.
//

#include <RHI/GraphicsPipeline.hpp>
#include <RHI/Device.hpp>
#include <Core.hpp>

namespace RHI
{
    namespace
    {
        //TODO: Move to shader compiler
        uint32_t CompileShader(uint32_t type, const char* src)
        {
            uint32_t shader = glCreateShader(type);
            glShaderSource(shader, 1, &src, nullptr);
            glCompileShader(shader);

            int32_t success;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (!success)
            {
                char log[512];
                glGetShaderInfoLog(shader, 512, nullptr, log);

                PANIC("ShaderCompiler: {}", log);
            }

            return shader;
        }

        GLuint CreateProgram(std::unordered_map<ShaderType, uint32_t> shaders)
        {
            uint32_t program = glCreateProgram();

            for (const auto& [type, shader] : shaders)
            {
                glAttachShader(program, shader);
            }

            glLinkProgram(program);

            int32_t success;
            glGetProgramiv(program, GL_LINK_STATUS, &success);

            if (!success)
            {
                char log[512];
                glGetProgramInfoLog(program, 512, nullptr, log);

                PANIC("Program link error: {},", log);
            }

            // TODO: potentially unsafe
            for (const auto& [type, shader] : shaders)
            {
                glDeleteShader(shader);
            }

            return program;
        }
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
        if (m_ShaderProgram)
            glDeleteProgram(m_ShaderProgram);
    }

    GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetInputAssembly(const VertexAttributeList& vertexAttributeList)
    {
        m_VertexAttributeList = vertexAttributeList;

        return *this;
    }

    GraphicsPipeline::Builder& GraphicsPipeline::Builder::AddShader(ShaderType type, std::string_view code)
    {
        m_ShaderSources.emplace(type, code);

        return *this;
    }

    GraphicsPipelineRef GraphicsPipeline::Builder::Build(Device& device)
    {
        for (const auto& [type, sourceCode] : m_ShaderSources)
        {
            uint32_t compilerShader = CompileShader(static_cast<uint32_t>(type), sourceCode.data());
            m_Shaders.emplace(type, compilerShader);
        }

        m_ShaderProgram = CreateProgram(m_Shaders);

        GraphicsPipelineRef pipeline = core::MakeIntrusive<GraphicsPipeline>();
        pipeline->m_VertexAttributeList = m_VertexAttributeList;
        pipeline->m_ShaderProgram = m_ShaderProgram;

        pipeline->m_Stride = CalculateStride(pipeline->m_VertexAttributeList);

        pipeline->m_CullMode = m_CullMode;
        pipeline->m_FrontFace = m_FrontFace;

        pipeline->m_DepthWrite = m_DepthWrite;
        pipeline->m_DepthTest = m_DepthTest;
        pipeline->m_DepthFunc = m_DepthFunc;

        return pipeline;
    }
}
