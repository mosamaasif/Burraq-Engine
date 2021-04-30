#include <BRQ.h>

#include "RenderContext.h"
#include "VulkanCommon.h"

#include "Application/Window.h"

namespace BRQ {

    RenderContext* RenderContext::s_RenderContext = VK_NULL_HANDLE;

    RenderContext::RenderContext() {
    
        m_CurrentIndex = 0;
        m_AcquiredImageIndex = 0;;
        m_DepthImage = {};
        m_DepthImageView = {};
        m_RenderPass = VK_NULL_HANDLE;
    }

    void RenderContext::Init(const Window* window) {

        s_RenderContext = new RenderContext();
        s_RenderContext->InitInternal(window);
    }

    void RenderContext::Destroy() {

        if (s_RenderContext) {

            s_RenderContext->DestroyInternal();
            delete s_RenderContext;
        }
    }

    void RenderContext::InitInternal(const Window* window) {

        CreateDevice(window);

        VulkanMemoryAllocatorCreateInfo info = {};
        info.Instance = m_Device.GetVulkanInstance();
        info.PhysicalDevice = m_Device.GetPhysicalDevice();
        info.Device = m_Device.GetDevice();

        VulkanMemoryAllocator::Init(info);

        CreateSwapchain();
        CreateDepthResources();

        CreateRenderPass();
    }

    void RenderContext::DestroyInternal() {

        DestroyRenderPass();

        DestroyDepthResources();
        DestroySwapchain();

        VulkanMemoryAllocator::Shutdown();

        DestroyDevice();
    }

    void RenderContext::CreateDevice(const Window* window) {

        m_Device.Init(window);
    }

    void RenderContext::DestroyDevice() {

        m_Device.Destroy();
    }
    void RenderContext::CreateSwapchain() {
      
        m_Swapchain.Init(m_Device);
    }

    void RenderContext::DestroySwapchain() {

        m_Swapchain.Destroy(m_Device);
    }

    VkResult RenderContext::AcquireImageIndex(const VkSemaphore& imageAvailableSemaphore) {

        U32 index = -1;

        VkResult result = vkAcquireNextImageKHR(m_Device.GetDevice(), m_Swapchain.GetSwapchain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &index);
        
        m_AcquiredImageIndex = index;

        return result;
    }

    void RenderContext::UpdateSwapchain() {

        m_Swapchain.Update(m_Device);
        UpdateDepthResources();
    }

    VkResult RenderContext::Present(const VkSemaphore& signalSemaphore) {

        VK::PresentInfo info = {};

        info.ImageIndex = m_AcquiredImageIndex;
        info.PresentQueue = m_Device.GetPresentationQueue();
        info.WaitSemaphore = signalSemaphore;
        info.Swapchain = m_Swapchain.GetSwapchain();

        VkResult result = VK::Present(info);

        m_CurrentIndex = (m_CurrentIndex + 1) % FRAME_LAG;

        return result;
    }

    void RenderContext::UpdateDepthResources() {

        DestroyDepthResources();
        CreateDepthResources();
    }


    void RenderContext::CreateDepthResources() {
        
        VkExtent2D extent = m_Swapchain.GetSwapchainExtent2D();

        VK::ImageCreateInfo info = {};
        info.ImageType = VK_IMAGE_TYPE_2D;
        info.Format = VK::FindDepthImageFormat(m_Device.GetPhysicalDevice());
        info.Extent.height = extent.height;
        info.Extent.width = extent.width;
        info.Extent.depth = 1;
        info.MipLevels = 1;
        info.ArrayLayers = 1;
        info.Samples = VK_SAMPLE_COUNT_1_BIT;
        info.Tiling = VK_IMAGE_TILING_OPTIMAL;
        info.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        info.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

        m_DepthImage = VK::CreateImage(info);

        VK::ImageViewCreateInfo view = {};
        view.Image = m_DepthImage.Image;
        view.ViewType = VK_IMAGE_VIEW_TYPE_2D;
        view.Format = VK::FindDepthImageFormat(m_Device.GetPhysicalDevice());
        view.SubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        view.SubresourceRange.baseMipLevel = 0;
        view.SubresourceRange.levelCount = 1;
        view.SubresourceRange.baseArrayLayer = 0;
        view.SubresourceRange.layerCount = 1;

        m_DepthImageView = VK::CreateImageView(m_Device.GetDevice(), view);
    }

    void RenderContext::DestroyDepthResources() {

        VK::DestroyImageView(m_Device.GetDevice(), m_DepthImageView);
        VK::DestroyImage(m_DepthImage);
    }

    void RenderContext::CreateRenderPass() {

        VK::AttachmentDescription depth = {};
        depth.Format = VK::FindDepthImageFormat(m_Device.GetPhysicalDevice());
        depth.Samples = VK_SAMPLE_COUNT_1_BIT;
        depth.LoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth.StoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth.StencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth.StencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth.FinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VK::AttachmentDescription color = {};
        color.Format = m_Device.GetSurfaceFormat().format;
        color.Samples = VK_SAMPLE_COUNT_1_BIT;
        color.LoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color.StoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        color.StencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color.StencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color.FinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VK::SubpassDescription subpass = {};
        subpass.ColorAttachments = { VK::GetAttachmentReference({ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }) };
        subpass.DepthStencilAttachment = VK::GetAttachmentReference({ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL });

        VK::SubpassDependency dependency = {};
        dependency.SrcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.DstSubpass = 0;
        dependency.SrcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.SrcAccessMask = 0;
        dependency.DstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.DstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VK::RenderPassCreateInfo info = {};
        info.Attachments = { VK::GetAttachmentDescription(color), VK::GetAttachmentDescription(depth) };
        info.Dependencies = { VK::GetSubpassDependency(dependency) };
        info.Subpasses = { VK::GetSubpassDescription(subpass) };

        m_RenderPass = VK::CreateRenderPass(m_Device.GetDevice(), info);
    }

    void RenderContext::DestroyRenderPass() {

        VK::DestroyRenderPass(m_Device.GetDevice(), m_RenderPass);
    }
}