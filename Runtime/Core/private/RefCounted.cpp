//
// Created by cosmin on 4/6/26.
//

#include <Core/RefCounted.hpp>

void RefCounted::AddRef()
{
    ++m_RefCount;
}

void RefCounted::Release()
{
    if (--m_RefCount == 0)
        delete this;
}

