#pragma once

#include "VulkanHelpers.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

namespace BRQ {

    class Window;

    class RenderContext {

    private:
        static RenderContext* s_RenderContext;

        VulkanDevice          m_Device;
        VulkanSwapchain       m_Swapchain;
        
        U32                   m_CurrentIndex;
        U32                   m_AcquiredImageIndex;

        VK::Image             m_DepthImage;
        VK::ImageView         m_DepthImageView;

        VkRenderPass          m_RenderPass;

    protected:
        RenderContext();
        RenderContext(const RenderContext& context) = default;

    public:
        ~RenderContext() = default;

        static RenderContext* GetInstance() { return s_RenderContext; }

        static void Init(const Window* window);
        static void Destroy();

        const VkInstance& GetVulkanInstance() const { return m_Device.GetVulkanInstance(); }
        const VkSurfaceKHR& GetSurface() const { return m_Device.GetSurface(); }
        const VkPhysicalDevice& GetPhysicalDevice() const { return m_Device.GetPhysicalDevice(); }
        const VkDevice& GetDevice() const { return m_Device.GetDevice(); }
        const VkSwapchainKHR& GetSwapchain() const { return m_Swapchain.GetSwapchain(); }

        const VkQueue& GetGraphicsQueue() const { return m_Device.GetGraphicsQueue(); }
        const VkQueue& GetPresentationQueue() const { return m_Device.GetPresentationQueue(); }
        const VkQueue& GetComputeQueue() const { return m_Device.GetComputeQueue(); }
        const VkQueue& GetTransferQueue() const { return m_Device.GetTransferQueue(); }

        U32 GetGraphicsQueueIndex() const { return m_Device.GetGraphicsQueueIndex(); }
        U32 GetPresentationQueueIndex() const { return m_Device.GetPresentationQueueIndex(); }
        U32 GetComputeQueueIndex() const { return m_Device.GetComputeQueueIndex(); }
        U32 GetTransferQueueIndex() const { return m_Device.GetTransferQueueIndex(); }

        const VkExtent2D& GetSwapchainExtent2D() const { return m_Swapchain.GetSwapchainExtent2D(); }

        const VkSurfaceFormatKHR& GetSurfaceFormat() const { return m_Device.GetSurfaceFormat(); }
        const VkFormat& GetSwapchainImageFormat() const { return m_Device.GetSurfaceFormat().format; }

        const std::vector<VK::ImageView>& GetImageViews() const { return m_Swapchain.GetSwapchainImageViews(); }
        VK::ImageView GetDepthView() const { return m_DepthImageView; }

        VkResult AcquireImageIndex(const VkSemaphore& imageAvailableSemaphore);
        U32 GetAcquiredImageIndex() const { return m_AcquiredImageIndex; }
        U32 GetCurrentIndex() const { return m_CurrentIndex; }
        U32 GetImageCount() const { return m_Device.GetSurfaceImageCount(); }

        const VkRenderPass& GetRenderPass() const { return m_RenderPass; }

        void UpdateSwapchain();

        VkResult Present(const VkSemaphore& signalSemaphore);

    private:
        void InitInternal(const Window* window);
        void DestroyInternal();

        void CreateDevice(const Window* window);
        void DestroyDevice();

        void CreateSwapchain();
        void DestroySwapchain();

        void CreateDepthResources();
        void DestroyDepthResources();
        void UpdateDepthResources();

        void CreateRenderPass();
        void DestroyRenderPass();
    };
}
