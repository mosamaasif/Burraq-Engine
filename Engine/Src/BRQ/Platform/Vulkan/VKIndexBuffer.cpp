#include <BRQ.h>

#include "VKDevice.h"
#include "VKIndexBuffer.h"
#include "Utilities/VulkanMemoryAllocator.h"

namespace BRQ {

    void VKIndexBuffer::Create(const std::vector<U32>& indexData) {

        auto vma = VulkanMemoryAllocator::GetInstance();

        VkBufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.size = sizeof(indexData[0]) * indexData.size();
        info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

        m_BufferInfo = vma->CreateBuffer(info, allocInfo);

        void* data = vma->MapMemory(m_BufferInfo);

        memcpy(data, indexData.data(), (size_t)info.size);

        vma->UnMapMemory(m_BufferInfo);
    }
    void VKIndexBuffer::Destroy() {

        auto vma = VulkanMemoryAllocator::GetInstance();

        vma->DestroyBuffer(m_BufferInfo);
    }
}