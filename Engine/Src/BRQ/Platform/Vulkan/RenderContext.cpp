#include <BRQ.h>

#include "RenderContext.h"
#include "VKCommon.h"

#include "Application/Window.h"

namespace BRQ {

    RenderContext* RenderContext::s_RenderContext = VK_NULL_HANDLE;

    RenderContext::RenderContext()
        : m_Window(nullptr), m_Instance(VK_NULL_HANDLE), m_Surface(VK_NULL_HANDLE), m_PhysicalDevice(VK_NULL_HANDLE),
        m_Device(VK_NULL_HANDLE), m_GraphicsAndPresentationQueue(VK_NULL_HANDLE), m_ComputeQueue(VK_NULL_HANDLE),
        m_TransferQueue(VK_NULL_HANDLE), m_GraphicsAndPresentationQueueIndex(VK_QUEUE_FAMILY_IGNORED), m_ComputeQueueIndex(VK_QUEUE_FAMILY_IGNORED),
        m_TransferQueueIndex(VK_QUEUE_FAMILY_IGNORED), m_SurfaceFormat({}), m_PresentMode(VK_PRESENT_MODE_FIFO_KHR),
        m_SwapchainExtent2D({ 0, 0 }), m_SurfaceComposite(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR), m_ImageCount(0),
        m_AcquiredImageIndex(-1), m_CurrentIndex(0), m_DepthImageView({}) { }

    void RenderContext::Init(const Window* window, const std::vector<std::pair<VK::QueueType, F32>>& requestedQueues) {

        s_RenderContext = new RenderContext();
        s_RenderContext->InitInternal(window, requestedQueues);
    }

    void RenderContext::Destroy() {

        if (s_RenderContext) {

            s_RenderContext->DestroyInternal();
            delete s_RenderContext;
        }
    }

    void RenderContext::InitInternal(const Window* window, const std::vector<std::pair<VK::QueueType, F32>>& requestedQueues) {

        m_Window = window;
        m_RequestedQueues = requestedQueues;

        CreateInstance();
        CreateSurface();
        SelectPhysicalDevice();
        CreateDevice();

        VulkanMemoryAllocatorCreateInfo info = {};
        info.Instance = m_Instance;
        info.PhysicalDevice = m_PhysicalDevice;
        info.Device = m_Device;

        VulkanMemoryAllocator::Init(info);

        GetRequestedQueues();
        CreateSwapchain();
        CreateDepthResources();
    }

    void RenderContext::DestroyInternal() {

        DestroyDepthResources();
        DestroySwapchain();

        VulkanMemoryAllocator::Shutdown();

        DestroyDevice();
        DestroySurface();
        DestroyInstance();
    }

    void RenderContext::CreateInstance() {

        VK::InstanceCreateInfo info = {};
        info.ApplicationName = "Minecraft";

        m_Instance = VK::CreateInstance(info);
    }

    void RenderContext::DestroyInstance() {

        VK::DestroyInstance(m_Instance);
    }

    void RenderContext::CreateSurface() {

        VK::SurfaceCreateInfo info = {};
        info.InstanceHandle = GetModuleHandle(nullptr);
        info.WindowHandle = (HWND)m_Window->GetNativeWindowHandle();

        m_Surface = VK::CreateSurface(m_Instance, info);
    }

    void RenderContext::DestroySurface() {

        VK::DestroySurface(m_Instance, m_Surface);
    }

    void RenderContext::SelectPhysicalDevice() {

        m_PhysicalDevice = VK::SeletePhysicalDevice(m_Instance, m_Surface);
    }

    void RenderContext::CreateDevice() {

        VK::DeviceCreateInfo info = {};
        info.QueueTypes = m_RequestedQueues;

        m_Device = VK::CreateDevice(m_PhysicalDevice, m_Surface, info);
    }

    void RenderContext::DestroyDevice() {

        VK::DestroyDevice(m_Device);
    }

    void RenderContext::GetRequestedQueues() {

        for (U64 i = 0; i < m_RequestedQueues.size(); i++) {

            VK::QueueType queue = m_RequestedQueues[i].first;

            if (queue == VK::QueueType::Graphics) {

                m_GraphicsAndPresentationQueueIndex = VK::GetQueueFamilyIndex(m_PhysicalDevice, m_Surface, queue);
                vkGetDeviceQueue(m_Device, m_GraphicsAndPresentationQueueIndex, 0, &m_GraphicsAndPresentationQueue);
            }
            else if (queue == VK::QueueType::Compute) {

                m_ComputeQueueIndex = VK::GetQueueFamilyIndex(m_PhysicalDevice, m_Surface, queue);
                vkGetDeviceQueue(m_Device, m_ComputeQueueIndex, 0, &m_ComputeQueue);
            }
            else if (queue == VK::QueueType::Tranfer) {

                m_TransferQueueIndex = VK::GetQueueFamilyIndex(m_PhysicalDevice, m_Surface, queue);
                vkGetDeviceQueue(m_Device, m_TransferQueueIndex, 0, &m_TransferQueue);
            }
        }
    }

    void RenderContext::CreateSwapchain() {
        
        VkSurfaceFormatKHR surfaceFormat = VK::ChooseSwapchainFormat(m_PhysicalDevice, m_Surface);
        VkPresentModeKHR presentMode = VK::ChooseSwapchainPresentMode(m_PhysicalDevice, m_Surface);
        VkExtent2D extent = VK::ChooseSwapchainExtent(m_PhysicalDevice, m_Surface, m_Window);

        auto capabilities = VK::GetSurfaceCapabilities(m_PhysicalDevice, m_Surface);

        U32 imageCount = capabilities.minImageCount + 1;

        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {

            imageCount = capabilities.maxImageCount;
        }

        VkCompositeAlphaFlagBitsKHR surfaceComposite;

        if (capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {

            surfaceComposite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        }
        else if (capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {

            surfaceComposite = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
        }
        else if (capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {

            surfaceComposite = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
        }
        else {

            surfaceComposite = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
        }

        m_SurfaceFormat = surfaceFormat;
        m_SwapchainExtent2D = extent;
        m_PresentMode = presentMode;
        m_ImageCount = imageCount;
        m_SurfaceComposite = surfaceComposite;

        VK::SwapchainCreateInfo info = {};
        info.Surface = m_Surface;
        info.MinImageCount = imageCount;
        info.SurfaceFormat = m_SurfaceFormat;
        info.ImageExtent = m_SwapchainExtent2D;
        info.ImageArrayLayers = 1;
        info.ImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.ImageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.PreTransform = capabilities.currentTransform;
        info.CompositeAlpha = surfaceComposite;
        info.PresentMode = m_PresentMode;
        info.Clipped = VK_TRUE;
        info.OldSwapchain = VK_NULL_HANDLE;

        m_SwapchainResult = VK::CreateSwapchain(m_Device, info);
    }

    void RenderContext::DestroySwapchain() {

        for (auto& view : m_SwapchainResult.SwapchainImageViews) {

            VK::DestroyImageView(m_Device, view);
        }

        m_SwapchainResult.SwapchainImages.clear();
        m_SwapchainResult.SwapchainImageViews.clear();

        VK::DestroySwapchain(m_Device, m_SwapchainResult.Swapchain);
    }

    U32 RenderContext::AcquireImageIndex(const VkSemaphore& imageAvailableSemaphore) {

        U32 index = -1;

        VkResult result = vkAcquireNextImageKHR(m_Device, m_SwapchainResult.Swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {

            m_SwapchainResult.SwapchainStatus = VK::SwapchainStatus::NotReady;
        }
        else {

            VK_CHECK(result);
        }
        
        return m_AcquiredImageIndex = index;
    }

    void RenderContext::UpdateSwapchain() {

        auto capabilities = VK::GetSurfaceCapabilities(m_PhysicalDevice, m_Surface);

        if (m_SwapchainExtent2D.height == capabilities.currentExtent.height && m_SwapchainExtent2D.width == capabilities.currentExtent.width) {

            return;
        }

        m_SwapchainExtent2D = capabilities.currentExtent;

        VK::SwapchainCreateInfo info = {};
        info.Surface = m_Surface;
        info.MinImageCount = m_ImageCount;
        info.SurfaceFormat = m_SurfaceFormat;
        info.ImageExtent = m_SwapchainExtent2D;
        info.ImageArrayLayers = 1;
        info.ImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.ImageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.PreTransform = capabilities.currentTransform;
        info.CompositeAlpha = m_SurfaceComposite;
        info.PresentMode = m_PresentMode;
        info.Clipped = VK_TRUE;
        info.OldSwapchain = m_SwapchainResult.Swapchain;

        for (auto& view : m_SwapchainResult.SwapchainImageViews) {

            VK::DestroyImageView(m_Device, view);
        }

        m_SwapchainResult.SwapchainImages.clear();
        m_SwapchainResult.SwapchainImageViews.clear();

        auto result = VK::UpdateSwapchain(m_Device, m_SwapchainResult.Swapchain, info);

        m_SwapchainResult.Swapchain = result.Swapchain;
        m_SwapchainResult.SwapchainImages = std::move(result.SwapchainImages);
        m_SwapchainResult.SwapchainImageViews = std::move(result.SwapchainImageViews);
        m_SwapchainResult.SwapchainStatus = result.SwapchainStatus;
    }

    void RenderContext::Present(const VkSemaphore& signalSemaphore) {

        VK::PresentInfo info = {};

        info.ImageIndex = m_AcquiredImageIndex;
        info.PresentQueue = m_GraphicsAndPresentationQueue;
        info.WaitSemaphore = signalSemaphore;
        info.Swapchain = m_SwapchainResult.Swapchain;

        VkResult result = VK::Present(info);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {

            m_SwapchainResult.SwapchainStatus = VK::SwapchainStatus::NotReady;
        }
        else {

            VK_CHECK(result);
        }

        m_CurrentIndex = (m_CurrentIndex + 1) % FRAME_LAG;
    }

    void RenderContext::UpdateDepthResources() {

        DestroyDepthResources();
        CreateDepthResources();
    }

#if 1
    void RenderContext::CreateDepthResources() {
        
        VK::ImageCreateInfo info = {};
        info.ImageType = VK_IMAGE_TYPE_2D;
        info.Format = VK::FindDepthImageFormat(m_PhysicalDevice);
        info.Extent.height = m_SwapchainExtent2D.height;
        info.Extent.width = m_SwapchainExtent2D.width;
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
        view.Image = m_DepthImage.ImageAllocation.Image;
        view.ViewType = VK_IMAGE_VIEW_TYPE_2D;
        view.Format = VK::FindDepthImageFormat(m_PhysicalDevice);
        view.SubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        view.SubresourceRange.baseMipLevel = 0;
        view.SubresourceRange.levelCount = 1;
        view.SubresourceRange.baseArrayLayer = 0;
        view.SubresourceRange.layerCount = 1;

        m_DepthImageView = VK::CreateImageView(m_Device, view);
    }

    void RenderContext::DestroyDepthResources() {

        VK::DestroyImageView(m_Device, m_DepthImageView);
        VK::DestroyImage(m_DepthImage);
    }
#endif
}