
#include <fstream>

#include <Core.hpp>
#include <RHI.hpp>
#include <Graphics/WorldRenderer.hpp>


namespace Gfx
{
    namespace
    {
        std::string LoadShaderCode(const std::string& path)
        {
            std::ifstream file(path, std::ios::binary);
            if (!file)
                PANIC("Failed to open file: {}", path);

            return std::string(std::istreambuf_iterator(file), std::istreambuf_iterator<char>());
        }
    }

    WorldRenderer::WorldRenderer(RHI::Device& device, uint32_t width, uint32_t height) :
        m_Device(device),
        m_Width(width),
        m_Height(height)
    {
        // Create offscreen color texture
        RHI::TextureDesc offscreenColorDesc;
        offscreenColorDesc.width = m_Width;
        offscreenColorDesc.height = m_Height;
        offscreenColorDesc.format = RHI::TextureFormat::RGBA16F;
        offscreenColorDesc.renderTarget = true;

        m_OffscreenTexture = m_Device.CreateTexture(offscreenColorDesc);

        // Create depth offscreen texture
        RHI::TextureDesc offscreenDepthDesc;
        offscreenDepthDesc.width = m_Width;
        offscreenDepthDesc.height = m_Height;
        offscreenDepthDesc.format = RHI::TextureFormat::Depth24Stencil8;
        offscreenDepthDesc.renderTarget = true;
        offscreenDepthDesc.mipmapped = false;
        RHI::TextureRef depthTexture = m_Device.CreateTexture(offscreenDepthDesc);

        RHI::RenderTargetDesc rtDesc;
        rtDesc.ColorAttachments = { m_OffscreenTexture };
        rtDesc.DepthAttachment = depthTexture;
        m_RenderTarget = m_Device.CreateRenderTarget(rtDesc);

        auto vertexCode = LoadShaderCode("shader/PBR.vert");
        auto fragmentCode = LoadShaderCode("shader/PBR.frag");

        RHI::VertexAttributeList vertexAttributeList;

        vertexAttributeList[0] = {
            RHI::VertexAttribute::Type::Vec3, offsetof(Vertex, px)
        };

        vertexAttributeList[1] = {
            RHI::VertexAttribute::Type::Vec3, offsetof(Vertex, nx)
        };

        vertexAttributeList[2] = {
            RHI::VertexAttribute::Type::Vec2, offsetof(Vertex, u)
        };

        vertexAttributeList[3] = {
            RHI::VertexAttribute::Type::Vec4, offsetof(Vertex, tx)
        };

        m_PBRPipeline = RHI::GraphicsPipeline::Builder()
            .SetInputAssembly(vertexAttributeList)
            .AddShader(RHI::ShaderType::Vertex, vertexCode)
            .AddShader(RHI::ShaderType::Fragment, fragmentCode)
            .SetCullMode(RHI::CullMode::Back)
            .SetDepthWrite(RHI::DepthWrite::Enable)
            .SetDepthTest(RHI::DepthTest::Enable)
            .SetDepthFunc(RHI::DepthFunc::Less)
            .Build(device);

        auto fullscreenVertexCode = LoadShaderCode("shader/Fullscreen.vert");
        auto fullscreenFragmentCode = LoadShaderCode("shader/Fullscreen.frag");

        m_FullscreenPipeline = RHI::GraphicsPipeline::Builder()
            .AddShader(RHI::ShaderType::Vertex, fullscreenVertexCode)
            .AddShader(RHI::ShaderType::Fragment, fullscreenFragmentCode)
            .SetDepthWrite(RHI::DepthWrite::Disable)
            .SetDepthTest(RHI::DepthTest::Disable)
            .Build(device);

        auto skyVert = LoadShaderCode("shader/Fullscreen.vert");
        auto skyFrag = LoadShaderCode("shader/Sky.frag");

        m_SkyPipeline = RHI::GraphicsPipeline::Builder()
            .AddShader(RHI::ShaderType::Vertex, skyVert)
            .AddShader(RHI::ShaderType::Fragment, skyFrag)
            .SetDepthWrite(RHI::DepthWrite::Disable)
            .SetDepthTest(RHI::DepthTest::Enable)
            .SetDepthFunc(RHI::DepthFunc::Lequal)
            .SetCullMode(RHI::CullMode::None)
            .Build(device);

        m_Environment = std::make_unique<EnvironmentMap>(
            m_Device,
            "assets/skybox/px.png",
            "assets/skybox/nx.png",
            "assets/skybox/py.png",
            "assets/skybox/ny.png",
            "assets/skybox/pz.png",
            "assets/skybox/nz.png"
        );
    }

    void WorldRenderer::Render(RHI::CommandList& cmd, const WorldView& world, const RenderView& view, float deltaTime)
    {
        cmd.BeginFrame();

        cmd.BeginRenderPass(m_RenderTarget);

        glDepthMask(GL_TRUE); // TODO: add additional argument for cmd.Clear
        cmd.Clear(0.5f, 0.6f, 0.8f, 1.0f);

        RenderScenePass(cmd, world, view);

        RenderSkyPass(cmd, view.view, view.projection);

        cmd.EndRenderPass();

        Compose(cmd);

        cmd.EndFrame();
    }

    void WorldRenderer::RenderScenePass(RHI::CommandList& cmd, const WorldView& world, const RenderView& view) const
    {
        cmd.BindGraphicsPipeline(m_PBRPipeline);

        cmd.SetUniform<glm::mat4>(1, view.view);
        cmd.SetUniform<glm::mat4>(2, view.projection);
        cmd.SetUniform<glm::vec3>(3, view.cameraPosition);

        glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
        glm::vec3 lightColor(1.0f, 0.9f, 0.8f);
        float lightIntensity = 1.0f;

        if (!world.lights.empty() && world.lights[0])
        {
            lightDir = world.lights[0]->direction;
            lightColor = world.lights[0]->color;
            lightIntensity = world.lights[0]->intensity;
        }

        cmd.SetUniform<glm::vec3>(4, lightDir);
        cmd.SetUniform<glm::vec3>(5, lightColor);
        cmd.SetUniform<float>(6, lightIntensity);

        for (const auto* instance : world.meshes)
        {
            if (!instance || !instance->mesh || !instance->material)
                continue;

            cmd.SetUniform<glm::mat4>(0, instance->transform);

            const auto& mat = *instance->material;

            cmd.SetUniform<glm::vec3>(7, mat.baseColor);
            cmd.SetUniform<float>(8, mat.metallicFactor);
            cmd.SetUniform<float>(9, mat.roughnessFactor);

            if (mat.albedo.handle)
                cmd.BindTexture(0, mat.albedo.handle);
            if (mat.metallic.handle)
                cmd.BindTexture(1, mat.metallic.handle);
            if (mat.roughness.handle)
                cmd.BindTexture(2, mat.roughness.handle);
            if (mat.normal.handle)
                cmd.BindTexture(3, mat.normal.handle);

            RHI::GeometryInfo geometryInfo
            {
                .vertexBuffer = instance->mesh->vertexBuffer,
                .indexBuffer = instance->mesh->indexBuffer,
                .indexCount = instance->mesh->indexCount,
            };


            cmd.Draw(geometryInfo);
        }
    }

    void WorldRenderer::Compose(RHI::CommandList& cmd)
    {
        cmd.BeginRenderPass();
        cmd.BindGraphicsPipeline(m_FullscreenPipeline);
        cmd.BindTexture(0, m_OffscreenTexture);
        cmd.DrawFullscreenTriangle();
        cmd.EndRenderPass();
    }

    void WorldRenderer::RenderSkyPass(RHI::CommandList& cmd, const glm::mat4& view, const glm::mat4& projection)
    {
        cmd.BindGraphicsPipeline(m_SkyPipeline);

        glm::mat4 invViewProjection = glm::inverse(projection * view);
        cmd.SetUniform<glm::mat4>(0, invViewProjection);
        cmd.BindTexture(0, m_Environment->GetCubemap());

        cmd.DrawFullscreenTriangle();
    }

}