//
// Created by cosmin on 4/6/26.
//

#pragma once

#include <cstdint>
#include <map>
#include <ranges>

namespace RHI
{
    struct VertexAttribute
    {
        enum class Type
        {
            Float,
            Vec2,
            Vec3,
            Vec4,
        };

        [[nodiscard]]
        uint32_t TypeSize() const;

        Type type;
        uint32_t offset;
    };

    // The key represents the location
    using VertexAttributeList = std::map<uint32_t, VertexAttribute>;

    inline uint32_t CalculateStride(VertexAttributeList& vertexAttributeList)
    {
        uint32_t stride = 0;
        uint32_t offset = 0;

        for (auto& attribute: vertexAttributeList | std::views::values)
        {
            attribute.offset = offset;
            offset += attribute.TypeSize();
            stride += attribute.TypeSize();
        }

        return stride;
    }
}
