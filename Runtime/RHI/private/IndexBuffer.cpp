//
// Created by cosmin on 5/3/26.
//

#include <RHI/IndexBuffer.hpp>

namespace RHI
{
    IndexBuffer::IndexBuffer(const void* data, uint32_t size) :
        Buffer(data, size)
    {

    }

    uint32_t IndexBuffer::GetCount() const
    {
        // Temp: should be uint16_t or uint32_t
        // return m_Size / sizeof(uint16_t);
        return m_Size / sizeof(uint32_t);
    }
}