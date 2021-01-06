#pragma once

#include "VKCommon.h"
#include "Graphics/Mesh.h"
#include "Utilities/VulkanMemoryAllocator.h"

namespace BRQ {

    class VKVertexBuffer {

    private:
        VulkanMemoryAllocator::BufferInfo m_BufferInfo;

    public:
        VKVertexBuffer() = default;
        ~VKVertexBuffer() = default;

        const VkBuffer& GetVertexBuffer() const { return m_BufferInfo.Buffer; }

        void Create(const std::vector<Vertex>& vertexData);
        void Destroy();
    };
}