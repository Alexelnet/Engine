//
// Created by cosmin on 4/28/26.
//

#include <cassert>

#include <../public/RHI/TransientAllocator.hpp>

namespace RHI
{
    void TransientAllocator::Init(uint32_t sizePerFrame)
    {
        // Rounding up (integer) to the next multiple of 256
        // Alternatively, could do: / kAlignment * kAlignment
        frameSize = (sizePerFrame + kAlignment - 1) & ~(kAlignment - 1);
        uint32_t totalSize = frameSize * kMaxFrames;

        glCreateBuffers(1, &bufferID);
        GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

        glNamedBufferStorage(bufferID, totalSize, nullptr, flags);
        persistentPtr = static_cast<uint8_t*>(glMapNamedBufferRange(bufferID, 0, totalSize, flags));
    }

    uint32_t TransientAllocator::Allocate(size_t size)
    {
        // Rounding up (integer) to the next multiple of 256
        // Alternatively, could do: / kAlignment * kAlignment
        uint32_t alignedSize = (static_cast<uint32_t>(size) + kAlignment - 1) & ~(kAlignment - 1);

        assert(currentOffset + alignedSize <= frameSize && "TransientAllocator overflow: frame size exceeded");

        uint32_t absoluteOffset = activeFrameIndex * frameSize + currentOffset;
        currentOffset += alignedSize;

        return absoluteOffset;
    }

    void TransientAllocator::Reset(uint32_t frameIndex)
    {
        activeFrameIndex = frameIndex;
        currentOffset = 0;
    }
}