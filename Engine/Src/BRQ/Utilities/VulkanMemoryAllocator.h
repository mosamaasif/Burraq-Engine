#pragma once

#include <vk_mem_alloc.h>

namespace BRQ {

    struct VulkanMemoryAllocatorCreateInfo {

        VkInstance          Instance = VK_NULL_HANDLE;
        VkPhysicalDevice    PhysicalDevice = VK_NULL_HANDLE;
        VkDevice            Device = VK_NULL_HANDLE;
    };

    class VulkanMemoryAllocator {

    public:
        struct BufferInfo {

            VkBuffer        Buffer = VK_NULL_HANDLE;
            VmaAllocation   Allocation = nullptr;
        };

        struct ImageInfo {

            VkImage         Image = VK_NULL_HANDLE;
            VmaAllocation   Allocation = nullptr;
        };

    private:
        static VulkanMemoryAllocator*   s_Instance;

        VmaAllocator                    m_Allocator;
 
    protected:
        VulkanMemoryAllocator();
        VulkanMemoryAllocator(const VulkanMemoryAllocator& vma) = delete;

    public:
        ~VulkanMemoryAllocator() = default;

        static void Init(const VulkanMemoryAllocatorCreateInfo& info = {});
        static void Shutdown();

        VmaAllocationInfo GetAllocationInfo(const BufferInfo& info) const;

        void* MapMemory(const BufferInfo& info);
        void UnMapMemory(const BufferInfo& info);

        static VulkanMemoryAllocator* GetInstance() { return s_Instance; }

        BufferInfo CreateBuffer(const VkBufferCreateInfo& createInfo, const VmaAllocationCreateInfo& allocInfo);
        void DestroyBuffer(BufferInfo& bufferInfo);

        ImageInfo CreateImage(const VkImageCreateInfo& createInfo, const VmaAllocationCreateInfo& allocInfo);
        void DestroyImage(ImageInfo& imageInfo);
    };
}