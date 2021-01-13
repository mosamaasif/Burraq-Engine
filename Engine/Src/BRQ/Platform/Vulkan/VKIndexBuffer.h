#pragma once

#include "VKCommon.h"
#include "Graphics/Mesh.h"
#include "Utilities/VulkanMemoryAllocator.h"

namespace BRQ {

    class VKIndexBuffer {

    private:
        VulkanMemoryAllocator::BufferInfo m_BufferInfo;

    public:
        VKIndexBuffer() = default;
        ~VKIndexBuffer() = default;

        const VkBuffer& GetIndexBuffer() const { return m_BufferInfo.Buffer; }

        const VulkanMemoryAllocator::BufferInfo& GetBufferInfo() const { return m_BufferInfo; }
        VulkanMemoryAllocator::BufferInfo& GetBufferInfo() { return m_BufferInfo; }

        void Create(const std::vector<U32>& IndexData);
        void Destroy();
    };
}