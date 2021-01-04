#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "VKCommon.h"

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

        const VkSurfaceKHR& GetSurface() const { return m_Surface; }

        VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(const VKDevice* device) const;
        std::vector<VkSurfaceFormatKHR> GetSurfaceFormat(const VKDevice* device) const;
        std::vector<VkPresentModeKHR> GetPresentMode(const VKDevice* device) const;

        void Create(const VKInstance* vkInstance, const Window* window);
        void Destroy();
    };
}