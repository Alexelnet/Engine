//
// Created by cosmin on 5/20/26.
//

#pragma once

#include <RHI.hpp>
#include <Graphics/Material.hpp>
#include <Graphics/Mesh.hpp>
#include <Graphics/EnvironmentMap.hpp>

namespace Gfx
{
    struct Light
    {
        glm::vec3 direction = {1, -1, 0};
        glm::vec3 color = {1,1,1};
        float intensity = 1.0f;
    };

    struct WorldView
    {
        std::vector<const MeshInstance*> meshes;
        std::vector<const Light*> lights;
    };

    struct RenderView
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 cameraPosition;

        uint32_t width = 1280;
        uint32_t height = 720;

        RHI::RenderTargetRef target;
    };

    class WorldRenderer
    {
    public:
        WorldRenderer(RHI::Device& device, uint32_t width, uint32_t height);

        void Render(RHI::CommandList& cmd, const WorldView& world, const RenderView& view, float deltaTime);

    private:

        void RenderScenePass(RHI::CommandList& cmd, const WorldView& world, const RenderView& view) const;
        void Compose(RHI::CommandList& cmd);
        void RenderSkyPass(RHI::CommandList& cmd, const glm::mat4& view, const glm::mat4& projection);

        
        RHI::Device& m_Device;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;

        RHI::TextureRef m_OffscreenTexture;
        RHI::RenderTargetRef m_RenderTarget;

        RHI::GraphicsPipelineRef m_PBRPipeline;
        RHI::GraphicsPipelineRef m_FullscreenPipeline;
        RHI::GraphicsPipelineRef m_SkyPipeline;

        RHI::TextureRef m_EnvironmentMap;
        std::unique_ptr<EnvironmentMap> m_Environment;
    };
}