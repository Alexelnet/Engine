//
// Created by cosmin on 4/28/26.
//

#pragma once

#include <cstddef>
#include <cstdint>

#include <glad/glad.h>

namespace RHI
{
    // https://wikis.khronos.org/opengl/Buffer_Object_Streaming
    // Multi-frame ring buffer allocator
    struct TransientAllocator
    {
        static constexpr uint32_t kMaxFrames = 3;
        static constexpr uint32_t kAlignment = 256;

        GLuint bufferID = 0;
        uint8_t* persistentPtr = nullptr;
        uint32_t frameSize = 0;
        uint32_t currentOffset = 0;
        uint32_t activeFrameIndex = 0;

        void Init(uint32_t sizePerFrame);

        uint32_t Allocate(size_t size);

        void Reset(uint32_t frameIndex);
    };
}