//
// Created by cosmin on 4/6/26.
//

#include <glad/glad.h>

#include <RHI/Buffer.hpp>

namespace RHI
{
    Buffer::Buffer(const void* data, uint32_t size) :
        m_Size(size)
    {
        glCreateBuffers(1, &m_ID);
        glNamedBufferData(m_ID, size, data, GL_STATIC_DRAW);
    }

    Buffer::~Buffer()
    {
        if (m_ID)
            glDeleteBuffers(1, &m_ID);
    }

    uint32_t Buffer::GetSize() const
    {
        return m_Size;
    }
}
