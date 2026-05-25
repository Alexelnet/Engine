//
// Created by cosmin on 5/3/26.
//

#include <print>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Core.hpp>
#include <RHI.hpp>
#include <Graphics/Camera.hpp>

#include <Graphics/WorldRenderer.hpp>

// #include "glm/gtx/dual_quaternion.hpp"

constexpr uint32_t g_Width = 1920;
constexpr uint32_t g_Height = 1080;

int main()
{
    // SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11,wayland");

    if (!SDL_Init(SDL_INIT_VIDEO))
        PANIC("SDL_Init Error: {}", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow("Engine", g_Width, g_Height, SDL_WINDOW_OPENGL);

    if (!window)
    {
        SDL_Quit();
        PANIC("SDL_CreateWindow Error: {}", SDL_GetError());
    }

    {
        RHI::Device device(window);

        Gfx::Camera camera;
        camera.position = glm::vec3(2.0f, 2.0f, 5.0f);
        camera.moveSpeed = 5.0f;
        camera.mouseSensitivity = 0.2f;

        bool mouseGrabbed = true;
        SDL_SetWindowRelativeMouseMode(window, true);

        float lastFrameTime = SDL_GetTicks() / 1000.0f;

        Gfx::WorldRenderer renderer(device, g_Width, g_Height);

        // Resources
        Gfx::Mesh mesh = Gfx::Mesh::Create(device, "assets/cube_cube_projection.fbx");
        Gfx::Mesh sofaMesh = Gfx::Mesh::Create(device, "assets/sofa_03_4k.fbx");

        Gfx::Texture::InitializeDefaults(device);

        // Cube
        auto albedoTex = Gfx::Texture::Load(device, "assets/bark_albedo.png", Gfx::TextureUsage::Color);
        auto normalTex = Gfx::Texture::Load(device, "assets/bark_normal.png", Gfx::TextureUsage::Normal);
        auto metallicTex = Gfx::Texture::Load(device, "assets/bark_metallic.png", Gfx::TextureUsage::Mask);
        auto roughnessTex = Gfx::Texture::Load(device, "assets/bark_roughness.png", Gfx::TextureUsage::Mask);

        Gfx::Material material
        {
            .albedo = albedoTex,
            .normal = normalTex,
            .metallic = metallicTex,
            .roughness = roughnessTex,
            .baseColor = glm::vec3(1.0f),
            .metallicFactor = 0.0f,
            .roughnessFactor = 1.0f,
        };

        Gfx::MeshInstance instance
        {
            .mesh = &mesh,
            .material = &material,
            .transform = glm::mat4(1.0f),
        };

        auto sofaAlbedoTex = Gfx::Texture::Load(device, "assets/sofa_03_diff_4k.jpg", Gfx::TextureUsage::Color);
        auto sofaNormalTex = Gfx::Texture::Load(device, "assets/sofa_03_nor_gl_4k.jpg", Gfx::TextureUsage::Normal);
        auto sofaMetallicTex = Gfx::Texture::Load(device, "assets/bark_metallic.png", Gfx::TextureUsage::Mask);
        auto sofaRoughnessTex = Gfx::Texture::Load(device, "assets/sofa_03_rough_4k.jpg", Gfx::TextureUsage::Mask);

        Gfx::Material sofaMaterial
        {
            .albedo = sofaAlbedoTex,
            .normal = sofaNormalTex,
            .metallic = sofaMetallicTex,
            .roughness = sofaRoughnessTex,
            .baseColor = glm::vec3(1.0f),
            .metallicFactor = 0.0f,
            .roughnessFactor = 1.0f,
        };


        // --- FIX: Proper Translate -> Rotate matrix ordering ---
        glm::mat4 transform = glm::mat4(1.0f);

        // 1. Translate FIRST (World Space Position)
        transform = glm::translate(transform, glm::vec3(5.0f, 0.0f, 0.0f));

        // 2. Rotate SECOND (Local Orientation)
        transform = glm::rotate(transform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        Gfx::MeshInstance sofaInstance
        {
            .mesh = &sofaMesh,
            .material = &sofaMaterial,
            .transform = transform, // This is now mathematically pristine?
            // .transform = glm::mat4(1.0f),
        };

        Gfx::Light light
        {
            // .direction = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)),
            // .direction = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)),
            .direction = glm::normalize(glm::vec3(0.5f, -1.0f, 0.3f)),
            .color = glm::vec3(1.0f, 0.9f, 0.8f),
            .intensity = 1.0f,
        };

        Gfx::WorldView world
        {
            .meshes = { &instance, &sofaInstance },
            .lights = { &light },
        };

        bool running = true;
        SDL_Event event;

        while (running)
        {
            float currentTime = SDL_GetTicks() / 1000.0f; // in seconds
            float deltaTime = currentTime - lastFrameTime;
            lastFrameTime = currentTime;
            deltaTime = std::min(deltaTime, 0.033f);

            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT)
                    running = false;
                else if (event.type == SDL_EVENT_KEY_DOWN)
                {
                    if (event.key.key == SDLK_ESCAPE)
                    {
                        if (mouseGrabbed)
                        {
                            SDL_SetWindowRelativeMouseMode(window, false);
                            mouseGrabbed = false;
                        }
                        else
                            running = false;
                    }
                    else if (event.key.key == SDLK_F1)
                    {
                        mouseGrabbed = !mouseGrabbed;
                        SDL_SetWindowRelativeMouseMode(window, mouseGrabbed);
                    }
                }
                else if (event.type == SDL_EVENT_MOUSE_MOTION && mouseGrabbed)
                    camera.Rotate(event.motion.xrel, event.motion.yrel);
            }

            const bool* keyState = SDL_GetKeyboardState(nullptr);

            if (mouseGrabbed)
            {
                camera.Move(
                    keyState[SDL_SCANCODE_W],
                    keyState[SDL_SCANCODE_S],
                    keyState[SDL_SCANCODE_A],
                    keyState[SDL_SCANCODE_D],
                    keyState[SDL_SCANCODE_SPACE],
                    keyState[SDL_SCANCODE_LCTRL],
                    deltaTime
                );
            }

            Gfx::RenderView view
            {
                .view = camera.GetViewMatrix(),
                .projection = camera.GetProjectionMatrix(static_cast<float>(g_Width) / g_Height),
                .cameraPosition = camera.position,
            };

            RHI::CommandList cmd(device);
            renderer.Render(cmd, world, view, deltaTime);

            SDL_GL_SwapWindow(window);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}