//
// Created by cosmin on 4/6/26.
//

#pragma once

#include <cstdint>

// RC base class
class RefCounted
{
public:
    void AddRef();

    void Release();

    // Development only or resource tracking
    [[nodiscard]]
    uint32_t GetRefCount() const
    {
        return m_RefCount;
    }

protected:
    virtual ~RefCounted() = default;

private:
    uint32_t m_RefCount = 0;
};