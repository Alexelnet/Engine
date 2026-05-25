//
// Created by cosmin on 5/3/26.
//

#include <print>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Core.hpp>
#include <RHI.hpp>

#include "stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


std::string LoadShaderCode(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file)
        core::panic("Failed to open file: {}", path);

    return std::string(std::istreambuf_iterator(file), std::istreambuf_iterator<char>());
}


// struct Vertex
// {
//     float px, py, pz;
//     float nx, ny, nz;
//     float u, v;
// };

struct Vertex
{
    float px, py, pz;
    float u, v;
};

inline std::pair<std::vector<Vertex>, std::vector<uint32_t>> LoadMesh(const std::string& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenNormals |
        aiProcess_FlipUVs
    );

    if (!scene || !scene->HasMeshes())
        throw std::runtime_error("Failed to load mesh: " + path);

    aiMesh* mesh = scene->mMeshes[0];


    std::vector<Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);

    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex v{};

        v.px = mesh->mVertices[i].x;
        v.py = mesh->mVertices[i].y;
        v.pz = mesh->mVertices[i].z;

        // if (mesh->HasNormals())
        // {
        //     v.nx = mesh->mNormals[i].x;
        //     v.ny = mesh->mNormals[i].y;
        //     v.nz = mesh->mNormals[i].z;
        // }

        if (mesh->mTextureCoords[0])
        {
            v.u = mesh->mTextureCoords[0][i].x;
            v.v = mesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(v);
    }

    // indices
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



    return {vertices, indices};
}



// Mesh data:

struct TexturedVertex
{
    float position[3];
    float uv[2];
};

static const TexturedVertex quadVertices[] = {
    {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
    {{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
    {{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}},
    {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}},
};

static const uint16_t quadIndices[] = {0, 1, 2, 2, 3, 0};


int main()
{
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11,wayland");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        PANIC("SDL_Init Error: {}", SDL_GetError());
    }

    // SDL_Window* window = SDL_CreateWindow("OpenGL 4.6", 1920, 1080, SDL_WINDOW_OPENGL);
    SDL_Window* window = SDL_CreateWindow("OpenGL 4.6", 1280, 720, SDL_WINDOW_OPENGL);
    if (!window)
    {
        SDL_Quit();
        PANIC("SDL_CreateWindow Error: {}", SDL_GetError());
    }

    // Temp lifetime scope
    {
        RHI::Device device(window);

        float vertices[] =
        {
            0.0f,  0.5f, 0.0f,
           -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f
        };

        // Buffer Setup
        RHI::VertexBufferRef vertexBuffer = device.CreateVertexBuffer(vertices, sizeof(vertices));

        // struct alignas(16) FrameData
        struct FrameData
        {
            float time = 1;
            float deltaTime = 60;
            // float _pad[2];
        } frameData;

        RHI::BufferRef frameDataBuffer = device.CreateBuffer(&frameData, sizeof(frameData));

        auto vertexCode = LoadShaderCode("shader/PBR.vert");
        auto fragmentCode = LoadShaderCode("shader/PBR.frag");

        RHI::CommandList cmd(device);

        // RHI::VertexAttributeList vertexAttributeList;
        // // vertexAttributeList[0] = { RHI::VertexAttribute::Type::Vec3, 0 };
        // vertexAttributeList[0] = { RHI::VertexAttribute::Type::Vec3, 0 }; // position
        // vertexAttributeList[1] = { RHI::VertexAttribute::Type::Vec2, 0 }; // uv


        RHI::VertexAttributeList vertexAttributeList;
        // vertexAttributeList[0] = { RHI::VertexAttribute::Type::Vec3, 0 };   // position
        // vertexAttributeList[1] = { RHI::VertexAttribute::Type::Vec2, 12 };  // uv

        // vertexAttributeList[0] = { RHI::VertexAttribute::Type::Vec3, offsetof(Vertex, px) };
        // vertexAttributeList[1] = { RHI::VertexAttribute::Type::Vec2, offsetof(Vertex, u) };

        // Best: make the RHI explicit
        vertexAttributeList[0] = { RHI::VertexAttribute::Type::Vec3, offsetof(Vertex, px) };
        vertexAttributeList[1] = { RHI::VertexAttribute::Type::Vec2, offsetof(Vertex, u) };
        // and make sure the binding stride is sizeof(Vertex) == 32


        // TODO: Make descriptor set abstraction and bindless
        RHI::GraphicsPipelineRef graphicsPipeline = RHI::GraphicsPipeline::Builder()
            .SetInputAssembly(vertexAttributeList)
            .AddShader(RHI::ShaderType::Vertex, vertexCode)
            .AddShader(RHI::ShaderType::Fragment, fragmentCode)
            .Build(device);

        auto fullscreenVertexCode = LoadShaderCode("shader/Fullscreen.vert");
        auto fullscreenFragmentCode = LoadShaderCode("shader/Fullscreen.frag");

        RHI::GraphicsPipelineRef fullscreenPipeline = RHI::GraphicsPipeline::Builder()
            .AddShader(RHI::ShaderType::Vertex, fullscreenVertexCode)
            .AddShader(RHI::ShaderType::Fragment, fullscreenFragmentCode)
            .Build(device);

        RHI::TextureDesc textureDesc
        {
            .width = 1920,
            .height = 1080,
            .format = RHI::TextureFormat::RGBA8,
            .renderTarget = true,
        };

        RHI::TextureRef texture = device.CreateTexture(textureDesc);

        RHI::RenderTargetDesc renderTargetDesc
        {
            .ColorAttachments = { texture },
        };

        auto renderTarget = device.CreateRenderTarget(renderTargetDesc);




        std::string path = "assets/box.png";

        int32_t w, h, channels;

        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);

        if (!data)
            PANIC("Failed to load texture: {}", path);

        RHI::TextureDesc desc;
        desc.width = w;
        desc.height = h;
        desc.format = RHI::TextureFormat::RGBA8;

        RHI::TextureRef boxTexture = device.CreateTexture(desc);

        device.UploadTexture(boxTexture, data, w, h, RHI::TextureFormat::RGBA8);

        stbi_image_free(data);



        // auto vb = device.CreateVertexBuffer(quadVertices, sizeof(quadVertices));
        // auto ib = device.CreateIndexBuffer(quadIndices, sizeof(quadIndices));

        auto meshData = LoadMesh("assets/cube_cube_projection.obj");

        auto vb = device.CreateVertexBuffer(
            meshData.first.data(),
            meshData.first.size() * sizeof(Vertex)
        );

        auto ib = device.CreateIndexBuffer(
            meshData.second.data(),
            meshData.second.size() * sizeof(uint32_t)
        );


        bool running = true;
        SDL_Event event;

        while (running)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT)
                    running = false;
                else if (event.type == SDL_EVENT_KEY_DOWN)
                {
                    if (event.key.key == SDLK_ESCAPE)
                        running = false;
                }
            }

            cmd.BeginFrame();

            float time = SDL_GetTicks() / 1000.0f;

            // {
            //     // Offscreen pass
            //     cmd.BeginRenderPass(renderTarget);
            //
            //     // Rendering
            //     // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            //     // glClear(GL_COLOR_BUFFER_BIT);
            //
            //     cmd.Clear(0.25, .1, 0.66, 1);
            //
            //     RHI::GeometryInfo geometryInfo
            //     {
            //         .vertexBuffer = vertexBuffer,
            //         .vertexOffset = 0,
            //         .vertexCount = 3,
            //     };
            //
            //     float deltaTime = 60;
            //
            //     cmd.BindGraphicsPipeline(graphicsPipeline);
            //     // cmd.SetUniform<float>(0, time);
            //     // cmd.SetUniform<float>(1, deltaTime);
            //
            //     frameData.time = time;
            //     frameData.deltaTime = deltaTime;
            //
            //     // PushConstants (Uniforms + small buffers for passing structs)
            //     cmd.PushConstants<FrameData>(0, frameData);
            //
            //     // Uniform Buffers
            //     // cmd.BindUniformBuffer(0, *frameDataBuffer);
            //     // glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FrameData), &frameData);
            //
            //     // StorageBuffers
            //     // cmd.BindStorageBuffer();
            //
            //     // Textures
            //     // cmd.BindTexture()
            //
            //     cmd.Draw(geometryInfo);
            //     // glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
            //
            //     cmd.EndRenderPass();
            // }

            // {
            //     // Composite pass
            //     cmd.BeginRenderPass();
            //
            //     cmd.BindGraphicsPipeline(fullscreenPipeline);
            //     cmd.BindTexture(0, texture);
            //     cmd.DrawFullscreenTriangle();
            //
            //     cmd.EndRenderPass();
            // }


            // {
            //     // Texture test pass
            //     cmd.BeginRenderPass();
            //     cmd.BindGraphicsPipeline(fullscreenPipeline);
            //     cmd.SetUniform<int32_t>(0, 0); // force sampler → unit 0
            //     cmd.BindTexture(0, boxTexture);
            //     cmd.DrawFullscreenTriangle();
            //     cmd.EndRenderPass();
            // }

            {
                float time = SDL_GetTicks() / 1000.0f;

                glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0, 0, 1));
                glm::mat4 view = glm::lookAt(
                    glm::vec3(0, 2, 5),  // camera position
                    glm::vec3(0, 0, 0),  // look at
                    glm::vec3(0, 1, 0)   // up
                );
                glm::mat4 proj = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

                glm::mat4 mvp = proj * view * model;

                // Texture test pass
                cmd.BeginRenderPass(renderTarget);

                cmd.Clear(0.1, 0.1, 0.1, 1);

                RHI::GeometryInfo geometryInfo
                {
                    .vertexBuffer = vb,
                    .vertexOffset = 0,
                    .vertexCount  = (uint32_t)meshData.first.size(),

                    .indexBuffer  = ib,
                    .indexOffset = 0,
                    .indexCount   = (uint32_t)meshData.second.size(),
                };

                cmd.BindGraphicsPipeline(graphicsPipeline);
                // cmd.SetUniform<int32_t>(0, 0); // force sampler → unit 0
                cmd.SetUniform<glm::mat4>(0, mvp);
                cmd.BindTexture(0, boxTexture);
                // cmd.DrawFullscreenTriangle();
                cmd.Draw(geometryInfo);
                cmd.EndRenderPass();
            }

            {
                // Composite pass
                cmd.BeginRenderPass();

                cmd.BindGraphicsPipeline(fullscreenPipeline);
                cmd.BindTexture(0, texture);
                cmd.DrawFullscreenTriangle();

                cmd.EndRenderPass();
            }



            cmd.EndFrame();

            // Swap buffers
            SDL_GL_SwapWindow(window);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}