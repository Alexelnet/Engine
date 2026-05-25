//
// Created by cosmin on 4/6/26.
//

#include <cassert>
#include <RHI/VertexAttributeList.hpp>

namespace RHI
{
    [[nodiscard]]
    uint32_t VertexAttribute::TypeSize() const
    {
        switch (type)
        {
            case Type::Float:
                return 4;
            case Type::Vec2:
                return 8;
            case Type::Vec3:
                return 12;
            case Type::Vec4:
                return 16;
            default:
                assert(false && "Unknown Type in TypeSize()");
        }
    }
}