//
// Created by cosmin on 4/6/26.
//

#pragma once

#include <Core/RefCounted.hpp>
#include <Core/intrusive_ptr.h>

namespace RHI
{
    class Buffer : public RefCounted
    {
        friend class Device;
        friend class CommandList;

    public:
        Buffer(const void* data, uint32_t size);
        ~Buffer() override;

        [[nodiscard]]
        uint32_t GetSize() const;

    protected:
        uint32_t m_Size = 0;

    private:
        uint32_t m_ID = 0;
    };

    using BufferRef = core::intrusive_ptr<Buffer>;
}