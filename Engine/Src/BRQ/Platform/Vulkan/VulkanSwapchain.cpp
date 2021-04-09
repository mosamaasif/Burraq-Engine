#include <BRQ.h>
#include "VulkanSwapchain.h"
#include "VulkanDevice.h"

namespace BRQ {

    VulkanSwapchain::VulkanSwapchain() {

        m_Swapchain = VK_NULL_HANDLE;
        m_SwapchainExtent2D = { 0, 0 };
    }

    void VulkanSwapchain::Init(const VulkanDevice& device) {

        m_SwapchainExtent2D = device.GetSurfaceExtent2D();

        VK::SwapchainCreateInfo info = {};
        info.Surface = device.GetSurface();
        info.MinImageCount = device.GetSurfaceImageCount();
        info.SurfaceFormat = device.GetSurfaceFormat();
        info.ImageExtent = m_SwapchainExtent2D;
        info.ImageArrayLayers = 1;
        info.ImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.ImageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.PreTransform = device.GetSurfaceTransform();
        info.CompositeAlpha = device.GetSurfaceComposite();
        info.PresentMode = device.GetSurfacePresentMode();
        info.Clipped = VK_TRUE;
        info.OldSwapchain = VK_NULL_HANDLE;

        VK::SwapchainResult result = VK::CreateSwapchain(device.GetDevice(), info);

        m_Swapchain = result.Swapchain;
        m_SwapchainImages = std::move(result.SwapchainImages);
        m_SwapchainImageViews = std::move(result.SwapchainImageViews);
    }

    void VulkanSwapchain::Destroy(const VulkanDevice& device) {

        for (auto& view : m_SwapchainImageViews) {

            VK::DestroyImageView(device.GetDevice(), view );
        }

        m_SwapchainImages.clear();
        m_SwapchainImageViews.clear();

        VK::DestroySwapchain(device.GetDevice(), m_Swapchain);
    }

    void VulkanSwapchain::Update(const VulkanDevice& device) {
        
        VkSurfaceCapabilitiesKHR capabilities = device.GetSurfaceCapabilities();

        U32 w = capabilities.currentExtent.width;
        U32 h = capabilities.currentExtent.height;

        if (w == 0 || h == 0) {

            // Minimized
            return;
        }
        else if (m_SwapchainExtent2D.width == w && m_SwapchainExtent2D.height == h) {

            // when maximizes
            return;
        }

        m_SwapchainExtent2D = { w, h };

        VK::SwapchainCreateInfo info = {};
        info.Surface = device.GetSurface();
        info.MinImageCount = device.GetSurfaceImageCount();
        info.SurfaceFormat = device.GetSurfaceFormat();
        info.ImageExtent = m_SwapchainExtent2D;
        info.ImageArrayLayers = 1;
        info.ImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.ImageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.PreTransform = device.GetSurfaceTransform();
        info.CompositeAlpha = device.GetSurfaceComposite();
        info.PresentMode = device.GetSurfacePresentMode();
        info.Clipped = VK_TRUE;
        info.OldSwapchain = m_Swapchain;

        for (auto& view : m_SwapchainImageViews) {

            VK::DestroyImageView(device.GetDevice(), view);
        }

        m_SwapchainImages.clear();
        m_SwapchainImageViews.clear();

        auto result = VK::UpdateSwapchain(device.GetDevice(), m_Swapchain, info);

        m_Swapchain = result.Swapchain;
        m_SwapchainImages = std::move(result.SwapchainImages);
        m_SwapchainImageViews = std::move(result.SwapchainImageViews);
    }
}

