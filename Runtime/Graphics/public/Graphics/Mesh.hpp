//
// Created by cosmin on 5/20/26.
//

#pragma once

#include <RHI/VertexBuffer.hpp>
#include <RHI/IndexBuffer.hpp>
#include <Graphics/Material.hpp>

namespace Gfx
{
    struct Vertex
    {
        // posiiton
        float px, py, pz;
        // normal
        float nx, ny, nz;
        // tex coord
        float u, v;
        // tangents
        float tx, ty, tz, tw;
    };

    struct Mesh
    {
        RHI::VertexBufferRef vertexBuffer;
        RHI::IndexBufferRef indexBuffer;
        uint32_t indexCount = 0;

        static Mesh Create(RHI::Device& device, std::string path);
    };

    struct MeshInstance
    {
        const Mesh* mesh = nullptr;
        const Material* material = nullptr;
        glm::mat4 transform{1.0f};
    };
}