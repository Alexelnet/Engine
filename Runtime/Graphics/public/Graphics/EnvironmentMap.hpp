//
// Created by cosmin on 5/20/26.
//

#pragma once

#include <RHI.hpp>
#include <string>
#include <array>
#include <RHI/Texture.hpp>

namespace Gfx
{
    class EnvironmentMap
    {
    public:
        EnvironmentMap(RHI::Device& device, const std::string& right, const std::string& left, const std::string& top, const std::string& bottom, const std::string& front, const std::string& back);

        [[nodiscard]]
        RHI::TextureRef GetCubemap() const
        {
            return m_Cubemap;
        }

    private:
        RHI::Device& m_Device;
        RHI::TextureRef m_Cubemap;
    };
}