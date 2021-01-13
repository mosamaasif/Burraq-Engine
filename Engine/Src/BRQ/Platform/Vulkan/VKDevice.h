#pragma once

#include <vulkan/vulkan.h>
#include "VKCommon.h"

namespace BRQ {

    class VKInstance;
    class VKSurface;

    class VKDevice {

    public:
        enum class QueueType {

            Presentation = 0,
            Graphics,
            Compute,
        };

    private:
        VkPhysicalDevice    m_PhysicalDevice;
        VkDevice            m_LogicalDevice;
        VkQueue             m_GraphicsQueue;
        VkQueue             m_PresentationQueue;
        U32                 m_PresentationQueueIndex;
        U32                 m_GraphicsQueueIndex;

    public:
        VKDevice();
        ~VKDevice() = default;

        void Create(const VKInstance* vkInstance, const VKSurface* surface);
        void Destroy();

        const VkPhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
        const VkDevice& GetLogicalDevice() const { return m_LogicalDevice; }

        VkQueue GetGraphicsFamilyQueue() const { return m_GraphicsQueue; }
        VkQueue GetPresentationFamilyQueue() const { return m_PresentationQueue; }

        void WaitDeviceIdle() const { VK_CHECK(vkDeviceWaitIdle(m_LogicalDevice)); }

        U32 GetGraphicsFamilyQueueIndex() const { return m_GraphicsQueueIndex; }
        U32 GetPresentationFamilyQueueIndex() const { return m_PresentationQueueIndex; }

    private:
        void SelectPhysicalDevice(const VkInstance* vkInstance, const VkSurfaceKHR* surface);
        void CreateLogicalDevice(const VkSurfaceKHR* surface);

        U32 GetQueueFamilyIndex(const VkPhysicalDevice* device, QueueType type, const VkSurfaceKHR* surface) const;
    };
}