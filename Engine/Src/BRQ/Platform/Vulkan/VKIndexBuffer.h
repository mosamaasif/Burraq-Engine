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

        void Create(const std::vector<U32>& IndexData);
        void Destroy();
    };
}