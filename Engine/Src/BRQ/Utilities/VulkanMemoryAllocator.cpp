#include <BRQ.h>

#define VMA_IMPLEMENTATION
#include "VulkanMemoryAllocator.h"

#include "Platform/Vulkan/VKDevice.h"
#include "Platform/Vulkan/VKInstance.h"

namespace BRQ {

    VulkanMemoryAllocator* VulkanMemoryAllocator::s_Instance = nullptr;

    VulkanMemoryAllocator::VulkanMemoryAllocator()
        : m_Allocator(VMA_NULL), m_Device(nullptr) { }

    void VulkanMemoryAllocator::Init(const VKInstance* instance, const VKDevice* device) {

        s_Instance = new VulkanMemoryAllocator();

        s_Instance->m_Device = device;

        VmaAllocatorCreateInfo info = {};
        info.physicalDevice = device->GetPhysicalDevice();
        info.device = device->GetLogicalDevice();
        info.instance = instance->GetInstance();

        VK_CHECK(vmaCreateAllocator(&info, &s_Instance->m_Allocator));
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
    }

}