//
// Created by cosmin on 4/6/26.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace RHI
{
    template<>
    CommandList& CommandList::SetUniform<float>(uint32_t location, const float& value)
    {
        glProgramUniform1f(m_CurrentGraphicsPipeline->m_ShaderProgram, location, value);
        return *this;
    }

    template<>
    CommandList& CommandList::SetUniform<double>(uint32_t location, const double& value)
    {
        glProgramUniform1d(m_CurrentGraphicsPipeline->m_ShaderProgram, location, value);
        return *this;
    }

    template<>
    CommandList& CommandList::SetUniform<int32_t>(uint32_t location, const int32_t& value)
    {
        glProgramUniform1i(m_CurrentGraphicsPipeline->m_ShaderProgram, location, value);
        return *this;
    }

    template<>
    CommandList& CommandList::SetUniform<uint32_t>(uint32_t location, const uint32_t& value)
    {
        glProgramUniform1ui(m_CurrentGraphicsPipeline->m_ShaderProgram, location, value);
        return *this;
    }

    template<>
    CommandList& CommandList::SetUniform<glm::vec2>(uint32_t location, const glm::vec2& value)
    {
        glProgramUniform2fv(m_CurrentGraphicsPipeline->m_ShaderProgram, location, 1, glm::value_ptr(value));
        return *this;
    }

    template<>
    CommandList& CommandList::SetUniform<glm::vec3>(uint32_t location, const glm::vec3& value)
    {
        glProgramUniform3fv(m_CurrentGraphicsPipeline->m_ShaderProgram, location, 1, glm::value_ptr(value));
        return *this;
    }

    template<>
    CommandList& CommandList::SetUniform<glm::vec4>(uint32_t location, const glm::vec4& value)
    {
        glProgramUniform4fv(m_CurrentGraphicsPipeline->m_ShaderProgram, location, 1, glm::value_ptr(value));
        return *this;
    }

    template<>
    CommandList& CommandList::SetUniform<glm::dvec2>(uint32_t location, const glm::dvec2& value)
    {
        glProgramUniform2dv(m_CurrentGraphicsPipeline->m_ShaderProgram, location, 1, glm::value_ptr(value));
        return *this;
    }

    template<>
    CommandList& CommandList::SetUniform<glm::dvec3>(uint32_t location, const glm::dvec3& value)
    {
        glProgramUniform3dv(m_CurrentGraphicsPipeline->m_ShaderProgram, location, 1, glm::value_ptr(value));
        return *this;
    }

    template<>
    CommandList& CommandList::SetUniform<glm::dvec4>(uint32_t location, const glm::dvec4& value)
    {
        glProgramUniform4dv(m_CurrentGraphicsPipeline->m_ShaderProgram, location, 1, glm::value_ptr(value));
        return *this;
    }

    template<>
    CommandList& CommandList::SetUniform<glm::mat3>(uint32_t location, const glm::mat3& value)
    {
        glProgramUniformMatrix3fv(m_CurrentGraphicsPipeline->m_ShaderProgram, location, 1, false, glm::value_ptr(value));
        return *this;
    }

    template<>
    CommandList& CommandList::SetUniform<glm::mat4>(uint32_t location, const glm::mat4& value)
    {
        glProgramUniformMatrix4fv(m_CurrentGraphicsPipeline->m_ShaderProgram, location, 1, false, glm::value_ptr(value));
        return *this;
    }

    template<>
    CommandList& CommandList::SetUniform<glm::dmat3>(uint32_t location, const glm::dmat3& value)
    {
        glProgramUniformMatrix3dv(m_CurrentGraphicsPipeline->m_ShaderProgram, location, 1, false, glm::value_ptr(value));
        return *this;
    }

    template<>
    CommandList& CommandList::SetUniform<glm::dmat4>(uint32_t location, const glm::dmat4& value)
    {
        glProgramUniformMatrix4dv(m_CurrentGraphicsPipeline->m_ShaderProgram, location, 1, false, glm::value_ptr(value));
        return *this;
    }
}