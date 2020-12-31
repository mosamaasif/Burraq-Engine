#include <BRQ.h>

#include "Application/Window.h"

#include "VKSurface.h"

#include "VKDevice.h"
#include "VKInstance.h"

namespace BRQ {

    VKSurface::VKSurface()
        : m_Surface(VK_NULL_HANDLE), m_VKInstance(VK_NULL_HANDLE) {}

    VkSurfaceCapabilitiesKHR VKSurface::GetVulkanSurfaceCapabilities(const VKDevice& device) const {

        VkSurfaceCapabilitiesKHR capabilities = {};
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetPhysicalDevice(), m_Surface, &capabilities));

        return capabilities;
    }

    std::vector<VkSurfaceFormatKHR> VKSurface::GetVulkanSurfaceFormat(const VKDevice& device) const {

        U32 formatCount = 0;

        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice(), m_Surface, &formatCount, nullptr));

        BRQ_CORE_ASSERT(formatCount);

        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysicalDevice(), m_Surface, &formatCount, &formats[0]));

        return std::move(formats);
    }

    std::vector<VkPresentModeKHR> VKSurface::GetVulkanPresentMode(const VKDevice& device) const {

        U32 presentCount = 0;

        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice(), m_Surface, &presentCount, nullptr));

        BRQ_CORE_ASSERT(presentCount);

        std::vector<VkPresentModeKHR> presentModes(presentCount);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysicalDevice(), m_Surface, &presentCount, &presentModes[0]));

        return std::move(presentModes);
    }

    void VKSurface::Create(const Window* window, const VKInstance* vkInstance) {

        m_VKInstance = vkInstance;

        VkWin32SurfaceCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        info.hwnd = (HWND)window->GetNativeWindowHandle();
        info.hinstance = GetModuleHandle(nullptr);

        VK_CHECK(vkCreateWin32SurfaceKHR(m_VKInstance->GetVulkanInstance(), &info, nullptr, &m_Surface));
    }

    void VKSurface::Destroy() {

        vkDestroySurfaceKHR(m_VKInstance->GetVulkanInstance(), m_Surface, nullptr);
    }
}