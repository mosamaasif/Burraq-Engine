#include <BRQ.h>

#include "VKDevice.h"
#include "VKSurface.h"
#include "VKSwapchain.h"
#include "VKSemaphore.h"
#include "VKFramebuffer.h"
#include "VKCommandBuffer.h"
#include "Application/Window.h"

namespace BRQ {

    VKSwapchain::VKSwapchain()
        : m_Swapchain(VK_NULL_HANDLE), m_Surface(nullptr), m_Device(nullptr), m_Window(nullptr),
        m_CurrentImageIndex(0), m_AcquiredNextImageIndex(0), m_SwapchainStatus(SwapchainStatus::NotReady),
        m_SurfaceFormat({}), m_Dimensions({}) { }

    void VKSwapchain::AcquireNextImageIndex(const VKSemaphore* imageAvailabeSemaphore) {
        
        VkResult result = vkAcquireNextImageKHR(m_Device->GetLogicalDevice(), m_Swapchain, UINT64_MAX, imageAvailabeSemaphore->GetSemaphore(), VK_NULL_HANDLE, &m_AcquiredNextImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {

            m_SwapchainStatus = SwapchainStatus::NotReady;
        }
        else {

            VK_CHECK(result);
        }
    }

    void VKSwapchain::Present(const VKSemaphore* signalSemaphore) {

        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.pWaitSemaphores = &signalSemaphore->GetSemaphore();
        info.waitSemaphoreCount = 1;
        info.swapchainCount = 1;
        info.pSwapchains = &m_Swapchain;
        info.pImageIndices = &m_AcquiredNextImageIndex;

        VkResult result = vkQueuePresentKHR(m_Device->GetPresentationFamilyQueue(), &info);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {

            m_SwapchainStatus = SwapchainStatus::NotReady;
        }
        else {

            VK_CHECK(result);
        }

        m_CurrentImageIndex = (m_CurrentImageIndex + 1) % FRAME_LAG;
    }

    void VKSwapchain::Create(const VKDevice* device, const VKSurface* surface, const Window* window) {

        m_Device = device;
        m_Surface = surface;
        m_Window = window;

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainFormat();
        VkPresentModeKHR presentMode = ChooseSwapchainPresentMode();
        VkExtent2D extent = ChooseSwapchainExtent2D();

        m_Dimensions = extent;
        m_SurfaceFormat = surfaceFormat;

        auto capabilities = m_Surface->GetSurfaceCapabilities(m_Device);

        U32 imageCount = capabilities.minImageCount + 1;

        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {

            imageCount = capabilities.maxImageCount;
        }

        BRQ_CORE_ASSERT(imageCount >= FRAME_LAG);

        VkSwapchainCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.surface = m_Surface->GetSurface();
        info.minImageCount = imageCount;
        info.imageFormat = surfaceFormat.format;
        info.imageColorSpace = surfaceFormat.colorSpace;
        info.imageExtent = extent;
        info.imageArrayLayers = 1;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        auto graphics = m_Device->GetGraphicsFamilyQueueIndex();
        auto present = m_Device->GetPresentationFamilyQueueIndex();

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
        info.oldSwapchain = m_Swapchain;

        VkSwapchainKHR old = m_Swapchain;

        VK_CHECK(vkCreateSwapchainKHR(m_Device->GetLogicalDevice(), &info, nullptr, &m_Swapchain));

        if (old != VK_NULL_HANDLE) {

            for (VkImageView view : m_SwapchainImageViews) {

                vkDestroyImageView(m_Device->GetLogicalDevice(), view, nullptr);
            }

            m_SwapchainImageViews.clear();
            m_SwapchainImages.clear();

            vkDestroySwapchainKHR(m_Device->GetLogicalDevice(), old, nullptr);
        }

        VK_CHECK(vkGetSwapchainImagesKHR(m_Device->GetLogicalDevice(), m_Swapchain, &imageCount, nullptr));
        m_SwapchainImages.resize(imageCount);
        m_SwapchainImageViews.resize(imageCount);

        VK_CHECK(vkGetSwapchainImagesKHR(m_Device->GetLogicalDevice(), m_Swapchain, &imageCount, &m_SwapchainImages[0]));

        for (U64 i = 0; i < m_SwapchainImages.size(); i++) {

            VkImageViewCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.image = m_SwapchainImages[i];
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

            VK_CHECK(vkCreateImageView(m_Device->GetLogicalDevice(), &info, nullptr, &m_SwapchainImageViews[i]));

            m_SwapchainStatus = SwapchainStatus::Ready;
            m_AcquiredNextImageIndex = 0;
            m_CurrentImageIndex = 0;
        }
    }

    void VKSwapchain::Destroy() {

        for (auto& i : m_SwapchainImageViews) {

            if (i) {

                vkDestroyImageView(m_Device->GetLogicalDevice(), i, nullptr);
            }
        }

        m_SwapchainImages.clear();
        m_SwapchainImageViews.clear();

        if (m_Swapchain) {

            vkDestroySwapchainKHR(m_Device->GetLogicalDevice(), m_Swapchain, nullptr);
            m_Swapchain = VK_NULL_HANDLE;
            m_SwapchainStatus = SwapchainStatus::NotReady;
        }
    }

    VkExtent2D VKSwapchain::ChooseSwapchainExtent2D() const {

        auto capabilities = m_Surface->GetSurfaceCapabilities(m_Device);

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

    VkSurfaceFormatKHR VKSwapchain::ChooseSwapchainFormat() const {

       auto formats = m_Surface->GetSurfaceFormat(m_Device);

       VkSurfaceFormatKHR format;

       if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {

           format = formats[0];
           format.format = VK_FORMAT_B8G8R8A8_UNORM;
       }
       else {

           format.format = VK_FORMAT_UNDEFINED;

           for (auto& candidate : formats) {

               switch (candidate.format) {

               case VK_FORMAT_R8G8B8A8_UNORM:
               case VK_FORMAT_B8G8R8A8_UNORM:
               case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
                   format = candidate;
                   break;

               default:
                   break;
               }

               if (format.format != VK_FORMAT_UNDEFINED) {

                   break;
               }
           }

           if (format.format == VK_FORMAT_UNDEFINED) {

               format = formats[0];
           }
       }
       return format;
    }

    VkPresentModeKHR VKSwapchain::ChooseSwapchainPresentMode() const {

        auto modes = m_Surface->GetPresentMode(m_Device);

        for (const auto& mode : modes) {

            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {

                return mode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    void VKSwapchain::CreateSwapchainFramebuffers(const VKRenderPass* renderPass) {

        m_SwapchainFramebuffers.resize(m_SwapchainImageViews.size());

        for (U64 i = 0; i < m_SwapchainImageViews.size(); i++) {

            m_SwapchainFramebuffers[i].Create(m_Device, this, renderPass, &m_SwapchainImageViews[i]);
        }
    }

    void VKSwapchain::DestroySwapchainFramebuffers() {

        for (auto& i : m_SwapchainFramebuffers) {

            i.Destroy();
        }

        m_SwapchainFramebuffers.clear();
    }
}