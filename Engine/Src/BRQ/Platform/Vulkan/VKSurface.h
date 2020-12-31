#pragma once

#include <BRQ.h>

namespace BRQ {

    class Window;
    class VKDevice;
    class VKInstance;

    class VKSurface {
        
    private:
        VkSurfaceKHR        m_Surface;
        const VKInstance*   m_VKInstance;

    public:
        VKSurface();
        ~VKSurface() = default;

        const VkSurfaceKHR& GetVulkanSurface() const { return m_Surface; }

        VkSurfaceCapabilitiesKHR GetVulkanSurfaceCapabilities(const VKDevice& device) const;
        std::vector<VkSurfaceFormatKHR> GetVulkanSurfaceFormat(const VKDevice& device) const;
        std::vector<VkPresentModeKHR> GetVulkanPresentMode(const VKDevice& device) const;

        void Create(const Window* window, const VKInstance* vkInstance);
        void Destroy();
    };
}