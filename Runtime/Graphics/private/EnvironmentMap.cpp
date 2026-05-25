//
// Created by cosmin on 5/20/26.
//

#include <Graphics/EnvironmentMap.hpp>
#include <stb/stb_image.h>
#include <Core.hpp>

namespace Gfx
{
    namespace
    {
        void LoadFace(RHI::Device& device, RHI::TextureRef cubemap, const std::string& path, uint32_t faceIndex)
        {
            int32_t w, h, channels;
            unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);

            if (!data)
                PANIC("Failed to load cubemap face: {}", path);

            device.UploadCubemapFace(cubemap, data, w, h, RHI::TextureFormat::RGBA8, faceIndex);

            stbi_image_free(data);
        }
    }

    EnvironmentMap::EnvironmentMap(RHI::Device& device, const std::string& right, const std::string& left, const std::string& top, const std::string& bottom, const std::string& front, const std::string& back) :
        m_Device(device)
    {
        int32_t w, h, channels;
        unsigned char* temp = stbi_load(right.c_str(), &w, &h, &channels, 4);

        if (!temp)
            PANIC("Failed to load right face: {}", right);

        stbi_image_free(temp);

        RHI::TextureDesc desc
        {
            .width = static_cast<uint32_t>(w),
            .height = static_cast<uint32_t>(h),
            .dimension = RHI::TextureDimension::TextureCube,
            .format = RHI::TextureFormat::RGBA8,
            .mipmapped = true,
            .generateMipmaps = true,
        };

        m_Cubemap = m_Device.CreateTexture(desc);

        LoadFace(m_Device, m_Cubemap, right,  0);
        LoadFace(m_Device, m_Cubemap, left,   1);
        LoadFace(m_Device, m_Cubemap, top,    2);
        LoadFace(m_Device, m_Cubemap, bottom, 3);
        LoadFace(m_Device, m_Cubemap, front,  4);
        LoadFace(m_Device, m_Cubemap, back,   5);
    }

}