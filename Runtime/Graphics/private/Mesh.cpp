//
// Created by cosmin on 5/20/26.
//

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <RHI/Device.hpp>
#include <Graphics/Mesh.hpp>

#include "Core.hpp"

namespace Gfx
{
    namespace
    {
        std::pair<std::vector<Vertex>, std::vector<uint32_t>> LoadMesh(const std::string& path)
        {
            Assimp::Importer importer;

            const aiScene* scene = importer.ReadFile(path,
                aiProcess_Triangulate |
                aiProcess_JoinIdenticalVertices |
                aiProcess_GenNormals |
                aiProcess_FlipUVs |
                aiProcess_CalcTangentSpace// |
                // aiProcess_PreTransformVertices
            );

            if (!scene || !scene->HasMeshes())
                PANIC("Failed to load mesh: {}", path);

            // Assuming there's a single mesh in the file, this is ok
            aiMesh* mesh = scene->mMeshes[0];

            std::vector<Vertex> vertices;
            vertices.reserve(mesh->mNumVertices);

            for (uint32_t i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex
                {
                    .px = mesh->mVertices[i].x,
                    .py = mesh->mVertices[i].y,
                    .pz = mesh->mVertices[i].z,
                };

                if (mesh->HasNormals())
                {
                    vertex.nx = mesh->mNormals[i].x;
                    vertex.ny = mesh->mNormals[i].y;
                    vertex.nz = mesh->mNormals[i].z;
                }
                else
                {
                    vertex.nx = 0.0f;
                    vertex.ny = 1.0f;
                    vertex.nz = 0.0f;
                }

                if (mesh->HasTextureCoords(0))
                {
                    vertex.u = mesh->mTextureCoords[0][i].x;
                    vertex.v = mesh->mTextureCoords[0][i].y;
                }

                if (mesh->HasTangentsAndBitangents())
                {
                    glm::vec3 T(
                        mesh->mTangents[i].x,
                        mesh->mTangents[i].y,
                        mesh->mTangents[i].z
                    );

                    glm::vec3 B(
                        mesh->mBitangents[i].x,
                        mesh->mBitangents[i].y,
                        mesh->mBitangents[i].z
                    );

                    glm::vec3 N(
                        mesh->mNormals[i].x,
                        mesh->mNormals[i].y,
                        mesh->mNormals[i].z
                    );

                    // TODO:
                    float handedness = (glm::dot(glm::cross(N, T), B) < 0.0f) ? -1.0f : 1.0f;

                    vertex.tx = T.x;
                    vertex.ty = T.y;
                    vertex.tz = T.z;
                    vertex.tw = handedness;
                }
                else
                {
                    vertex.tx = 1.0f;
                    vertex.ty = 0.0f;
                    vertex.tz = 0.0f;
                    vertex.tw = 1.0f;
                }

                vertices.push_back(vertex);
            }

            std::vector<uint32_t> indices;
            indices.reserve(mesh->mNumFaces * 3);

            for (uint32_t i = 0; i < mesh->mNumFaces; i++)
            {
                const aiFace& face = mesh->mFaces[i];
                assert(face.mNumIndices == 3);
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }

            return { vertices, indices };
        }
    }

    Mesh Mesh::Create(RHI::Device& device, std::string path)
    {
        auto meshData = LoadMesh(path);
        auto vertexBuffer = device.CreateVertexBuffer(meshData.first.data(), meshData.first.size() * sizeof(Vertex));
        auto indexBuffer = device.CreateIndexBuffer(meshData.second.data(), meshData.second.size() * sizeof(uint32_t));

        return Mesh
        {
            .vertexBuffer = vertexBuffer,
            .indexBuffer = indexBuffer,
            .indexCount = static_cast<uint32_t>(meshData.second.size()),
        };
    }
}
