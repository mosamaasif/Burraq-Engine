#pragma once

#include <BRQ.h>

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

    public:
        VKDevice();
        ~VKDevice() = default;

        void Create(const VKInstance* vkInstance, const VKSurface* surface);
        void Destroy();

        const VkPhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
        const VkDevice& GetLogicalDevice() const { return m_LogicalDevice; }

        U32 GetQueueFamilyIndex(const VkPhysicalDevice& device, QueueType type, const VkSurfaceKHR& surface) const;
    private:
        void SelectPhysicalDevice(const VkInstance& vkInstance, const VkSurfaceKHR& surface);
        void CreateLogicalDevice(const VkSurfaceKHR& surface);

    };
}