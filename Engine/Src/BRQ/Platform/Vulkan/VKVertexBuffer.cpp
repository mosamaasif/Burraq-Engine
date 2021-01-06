#include <BRQ.h>

#include "VKDevice.h"
#include "VKVertexBuffer.h"
#include "Utilities/VulkanMemoryAllocator.h"

namespace BRQ {

    void VKVertexBuffer::Create(const std::vector<Vertex>& vertexData) {

        auto vma = VulkanMemoryAllocator::GetInstance();

        VkBufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.size = sizeof(vertexData[0]) * vertexData.size();
        info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

        m_BufferInfo = vma->CreateBuffer(info, allocInfo);

        void* data = vma->MapMemory(m_BufferInfo);

        memcpy(data, vertexData.data(), (size_t)info.size);

        vma->UnMapMemory(m_BufferInfo);
    }
    void VKVertexBuffer::Destroy() {

        auto vma = VulkanMemoryAllocator::GetInstance();

        vma->DestroyBuffer(m_BufferInfo);
    }
}