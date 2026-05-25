//
// Created by cosmin on 4/6/26.
//

#pragma once

#include <cstdint>
#include <cstddef>

#include "Buffer.hpp"

namespace RHI
{
    //TODO: assign stride
    class VertexBuffer : public Buffer
    {
    public:
        VertexBuffer(const void* data, uint32_t size);

        [[nodiscard]]
        uint32_t GetStride() const;

        [[nodiscard]]
        size_t GetCapacity() const;

    private:
        uint32_t m_Stride = 0;
    };

    using VertexBufferRef = core::intrusive_ptr<VertexBuffer>;
}
