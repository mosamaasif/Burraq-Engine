#include <BRQ.h>

#include "VKDevice.h"
#include "VKSurface.h"
#include "VKSwapChain.h"
#include "Application/Window.h"

namespace BRQ {

    VKSwapChain::VKSwapChain()
        : m_SwapChain(VK_NULL_HANDLE), m_Surface(nullptr), m_Device(nullptr), m_Window(nullptr) { }

    void VKSwapChain::Create(const VKSurface* surface, const VKDevice* device, const Window* window) {

        m_Device = device;
        m_Surface = surface;
        m_Window = window;

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapChainFormat();
        VkPresentModeKHR presentMode = ChooseSwapChainPresentMode();
        VkExtent2D extent = ChooseSwapChainExtent2D();

        auto capabilities = m_Surface->GetVulkanSurfaceCapabilities(*m_Device);


        U32 imageCount = capabilities.minImageCount + 1;

        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {

            imageCount = capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.surface = m_Surface->GetVulkanSurface();
        info.minImageCount = imageCount;
        info.imageFormat = surfaceFormat.format;
        info.imageColorSpace = surfaceFormat.colorSpace;
        info.imageExtent = extent;
        info.imageArrayLayers = 1;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        auto graphics = m_Device->GetQueueFamilyIndex(m_Device->GetPhysicalDevice(), VKDevice::QueueType::Graphics, m_Surface->GetVulkanSurface());
        auto present = m_Device->GetQueueFamilyIndex(m_Device->GetPhysicalDevice(), VKDevice::QueueType::Presentation, m_Surface->GetVulkanSurface());

        U32 queueFamilyIndices[] = { graphics, present };

        if (graphics != present) {

            info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            info.queueFamilyIndexCount = 2;
            info.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        info.preTransform = capabilities.currentTransform;
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.presentMode = presentMode;
        info.clipped = VK_TRUE;
        info.oldSwapchain = VK_NULL_HANDLE;

        VK_CHECK(vkCreateSwapchainKHR(m_Device->GetLogicalDevice(), &info, nullptr, &m_SwapChain));

        VK_CHECK(vkGetSwapchainImagesKHR(m_Device->GetLogicalDevice(), m_SwapChain, &imageCount, nullptr));
        m_SwapChainImages.resize(imageCount);
        m_SwapChainImageViews.resize(imageCount);

        VK_CHECK(vkGetSwapchainImagesKHR(m_Device->GetLogicalDevice(), m_SwapChain, &imageCount, &m_SwapChainImages[0]));

        for (U64 i = 0; i < m_SwapChainImages.size(); i++) {

            VkImageViewCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.image = m_SwapChainImages[i];
            info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            info.format = surfaceFormat.format;
            info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            info.subresourceRange.baseMipLevel = 0;
            info.subresourceRange.levelCount = 1;
            info.subresourceRange.baseArrayLayer = 0;
            info.subresourceRange.layerCount = 1;

            VK_CHECK(vkCreateImageView(m_Device->GetLogicalDevice(), &info, nullptr, &m_SwapChainImageViews[i]));
        }

    }

    void VKSwapChain::Destroy() {

        vkDestroySwapchainKHR(m_Device->GetLogicalDevice(), m_SwapChain, nullptr);
    }

    VkExtent2D VKSwapChain::ChooseSwapChainExtent2D() {

        auto capabilities = m_Surface->GetVulkanSurfaceCapabilities(*m_Device);

        if (capabilities.currentExtent.width != UINT32_MAX) {

            return capabilities.currentExtent;
        }
        else {

           U32 height = m_Window->GetHeight();
           U32 width = m_Window->GetWidth();

            VkExtent2D extent = { (U32)width, (U32)height };

            extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
            extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));

            return extent;
        }
    }

    VkSurfaceFormatKHR VKSwapChain::ChooseSwapChainFormat() {

       auto formats = m_Surface->GetVulkanSurfaceFormat(*m_Device);

       for (const auto& format : formats) {

           if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
               return format;
           }
       }

       return formats[0];
    }

    VkPresentModeKHR VKSwapChain::ChooseSwapChainPresentMode() {

        auto modes = m_Surface->GetVulkanPresentMode(*m_Device);

        for (const auto& mode : modes) {

            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {

                return mode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }
}