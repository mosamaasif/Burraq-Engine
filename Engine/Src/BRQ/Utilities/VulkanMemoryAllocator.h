#pragma once

#include <vk_mem_alloc.h>

namespace BRQ {

    class VKInstance;
    class VKDevice;

    class VulkanMemoryAllocator {

    public:
        struct BufferInfo {

            VkBuffer Buffer = VK_NULL_HANDLE;
            VmaAllocation Allocation = nullptr;
        };

    private:
        static VulkanMemoryAllocator*   s_Instance;
        VmaAllocator                    m_Allocator;
        const VKDevice*                 m_Device;

    protected:
        VulkanMemoryAllocator();
        VulkanMemoryAllocator(const VulkanMemoryAllocator& vma) = delete;

    public:
        ~VulkanMemoryAllocator() = default;

        static void Init(const VKInstance* instance, const VKDevice* device);
        static void Shutdown();

        const VKDevice* GetDevice() const { return m_Device; }

        VmaAllocationInfo GetAllocationInfo(const BufferInfo& info) const;

        void* MapMemory(const BufferInfo& info);
        void UnMapMemory(const BufferInfo& info);

        static VulkanMemoryAllocator* GetInstance() { return s_Instance; }

        BufferInfo CreateBuffer(const VkBufferCreateInfo& createInfo, const VmaAllocationCreateInfo& allocInfo);
        void DestroyBuffer(BufferInfo& bufferInfo);

    };
}