#include <BRQ.h>

#define VMA_IMPLEMENTATION
#include "VulkanMemoryAllocator.h"

#include "Platform/Vulkan/VKCommon.h"

namespace BRQ {

    VulkanMemoryAllocator* VulkanMemoryAllocator::s_Instance = nullptr;

    VulkanMemoryAllocator::VulkanMemoryAllocator()
        : m_Allocator(VMA_NULL) { }

    void VulkanMemoryAllocator::Init(const VulkanMemoryAllocatorCreateInfo& info) {

        s_Instance = new VulkanMemoryAllocator();

        VmaAllocatorCreateInfo createInfo = {};
        createInfo.instance = info.Instance;
        createInfo.physicalDevice = info.PhysicalDevice;
        createInfo.device = info.Device;

        VK_CHECK(vmaCreateAllocator(&createInfo, &s_Instance->m_Allocator));
    }

    void VulkanMemoryAllocator::Shutdown() {

        if (s_Instance) {

            vmaDestroyAllocator(s_Instance->m_Allocator);
            delete s_Instance;
        }
    }

    VmaAllocationInfo VulkanMemoryAllocator::GetAllocationInfo(const BufferInfo& info) const {

        VmaAllocationInfo allocInfo = {};
        vmaGetAllocationInfo(m_Allocator, info.Allocation, &allocInfo);

        return allocInfo;
    }

    void* VulkanMemoryAllocator::MapMemory(const BufferInfo& info) {

        void* data = nullptr;
        VK_CHECK(vmaMapMemory(m_Allocator, info.Allocation, &data));

        return data;
    }

    void VulkanMemoryAllocator::UnMapMemory(const BufferInfo& info) {

        vmaUnmapMemory(m_Allocator, info.Allocation);
    }

    VulkanMemoryAllocator::BufferInfo VulkanMemoryAllocator::CreateBuffer(const VkBufferCreateInfo& createInfo, const VmaAllocationCreateInfo& allocInfo) {

        BufferInfo bufferInfo = {};

        VK_CHECK(vmaCreateBuffer(m_Allocator, &createInfo, &allocInfo, &bufferInfo.Buffer, &bufferInfo.Allocation, nullptr));

        return bufferInfo;
    }

    void VulkanMemoryAllocator::DestroyBuffer(BufferInfo& bufferInfo) {

        vmaDestroyBuffer(m_Allocator, bufferInfo.Buffer, bufferInfo.Allocation);

        bufferInfo.Buffer = VK_NULL_HANDLE;
        bufferInfo.Allocation = nullptr;
    }

    VulkanMemoryAllocator::ImageInfo VulkanMemoryAllocator::CreateImage(const VkImageCreateInfo& createInfo, const VmaAllocationCreateInfo& allocInfo) {

        ImageInfo info = {};

        VK_CHECK(vmaCreateImage(m_Allocator, &createInfo, &allocInfo, &info.Image, &info.Allocation, nullptr));

        return info;
    }

    void VulkanMemoryAllocator::DestroyImage(ImageInfo& imageInfo) {

        vmaDestroyImage(m_Allocator, imageInfo.Image, imageInfo.Allocation);

        imageInfo.Image = VK_NULL_HANDLE;
        imageInfo.Allocation = nullptr;
    }
}