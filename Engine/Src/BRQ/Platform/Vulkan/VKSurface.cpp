#include <BRQ.h>

#include "Application/Window.h"

#include "VKSurface.h"
#include "VKDevice.h"
#include "VKInstance.h"

namespace BRQ {

    VKSurface::VKSurface()
        : m_Surface(VK_NULL_HANDLE), m_VKInstance(nullptr) {}

    VkSurfaceCapabilitiesKHR VKSurface::GetSurfaceCapabilities(const VKDevice* device) const {

        VkSurfaceCapabilitiesKHR capabilities = {};
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice(), m_Surface, &capabilities));

        return capabilities;
    }

    std::vector<VkSurfaceFormatKHR> VKSurface::GetSurfaceFormat(const VKDevice* device) const {

        U32 formatCount = 0;

        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice(), m_Surface, &formatCount, nullptr));

        BRQ_CORE_ASSERT(formatCount);

        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice(), m_Surface, &formatCount, &formats[0]));

        return std::move(formats);
    }

    std::vector<VkPresentModeKHR> VKSurface::GetPresentMode(const VKDevice* device) const {

        U32 presentCount = 0;

        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice(), m_Surface, &presentCount, nullptr));

        BRQ_CORE_ASSERT(presentCount);

        std::vector<VkPresentModeKHR> presentModes(presentCount);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice(), m_Surface, &presentCount, &presentModes[0]));

        return std::move(presentModes);
    }

    void VKSurface::Create(const VKInstance* vkInstance, const Window* window) {

        m_VKInstance = vkInstance;

        VkWin32SurfaceCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        info.hwnd = (HWND)window->GetNativeWindowHandle();
        info.hinstance = GetModuleHandle(nullptr);

        VK_CHECK(vkCreateWin32SurfaceKHR(m_VKInstance->GetInstance(), &info, nullptr, &m_Surface));
    }

    void VKSurface::Destroy() {

        if (m_Surface) {

            vkDestroySurfaceKHR(m_VKInstance->GetInstance(), m_Surface, nullptr);
            m_Surface = VK_NULL_HANDLE;
        }
    }
}