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

        const VulkanDevice& GetVulkanDevice() const { return m_Device; }
        const VulkanSwapchain& GetVulkanSwapchain() const { return m_Swapchain; }

        VK::ImageView GetDepthView() const { return m_DepthImageView; }

        VkResult AcquireImageIndex(const VkSemaphore& imageAvailableSemaphore);
        U32 GetAcquiredImageIndex() const { return m_AcquiredImageIndex; }

        U32 GetCurrentIndex() const { return m_CurrentIndex; }

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
