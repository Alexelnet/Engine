//
// Created by cosmin on 5/20/26.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Gfx
{
    struct Camera
    {
        glm::vec3 position = {0.0f, 2.0f, 5.0f};
        glm::vec3 up = {0.0f, 1.0f, 0.0f};

        float yaw = -90.0f;
        float pitch = 0.0f;

        float FOV = glm::radians(60.0f);
        float nearPlane = 0.01f;
        float farPlane = 1000.0f;

        float mouseSensitivity = 0.1f;
        float moveSpeed = 5.0f;

        [[nodiscard]]
        glm::vec3 GetForward() const
        {
            float yawRad = glm::radians(yaw);
            float pitchRad = glm::radians(pitch);

            return glm::normalize(glm::vec3
            {
                std::cos(yawRad) * std::cos(pitchRad),
                std::sin(pitchRad),
                std::sin(yawRad) * std::cos(pitchRad),
            });
        }

        [[nodiscard]]
        glm::vec3 GetRight() const
        {
            return glm::normalize(glm::cross(GetForward(), up));
        }

        [[nodiscard]]
        glm::mat4 GetViewMatrix() const
        {
            glm::vec3 forward = GetForward();
            return glm::lookAt(position, position + forward, up);
        }

        [[nodiscard]]
        glm::mat4 GetProjectionMatrix(float aspectRatio) const
        {
            return glm::perspective(FOV, aspectRatio, nearPlane, farPlane);
        }

        [[nodiscard]]
        glm::mat4 GetViewProjection(float aspectRatio) const
        {
            return GetProjectionMatrix(aspectRatio) * GetViewMatrix();
        }

        void Rotate(float dx, float dy)
        {
            yaw += dx * mouseSensitivity;
            pitch -= dy * mouseSensitivity;
            pitch = std::clamp(pitch, -89.99f, 89.99f);
        }

        void Move(bool forward, bool back, bool left, bool right, bool up, bool down, float deltaTime)
        {
            float speed = moveSpeed * deltaTime;

            if (forward)
                position += GetForward() * speed;
            if (back)
                position -= GetForward() * speed;
            if (left)
                position -= GetRight() * speed;
            if (right)
                position += GetRight() * speed;
            if (up)
                position += glm::vec3{0, 1, 0} * speed;
            if (down)
                position -= glm::vec3{0, 1, 0} * speed;
        }
    };
}