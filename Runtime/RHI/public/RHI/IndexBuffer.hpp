//
// Created by cosmin on 5/3/26.
//

#pragma once

#include <cstdint>

#include <RHI/Buffer.hpp>

namespace RHI
{
    class IndexBuffer : public Buffer
    {
    public:
        IndexBuffer(const void* data, uint32_t size);

        [[nodiscard]]
        uint32_t GetCount() const;
    };

    using IndexBufferRef = core::intrusive_ptr<IndexBuffer>;
}