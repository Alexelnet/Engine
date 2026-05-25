//
// Created by cosmin on 4/6/26.
//

#include <RHI/VertexBuffer.hpp>

namespace RHI
{
    VertexBuffer::VertexBuffer(const void* data, uint32_t size) :
        Buffer(data, size)
    {

    }

    [[nodiscard]]
    uint32_t VertexBuffer::GetStride() const
    {
        return m_Stride;
    }

    [[nodiscard]]
    size_t VertexBuffer::GetCapacity() const
    {
        return m_Size / m_Stride;
    }
}