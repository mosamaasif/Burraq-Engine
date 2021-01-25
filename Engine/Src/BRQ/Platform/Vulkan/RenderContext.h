#pragma once

#include "VKInitializers.h"

#define FRAME_LAG   2

namespace BRQ {

    class Window;

    class RenderContext {

    private:
        static RenderContext*                       s_RenderContext;

        const Window*                               m_Window;

        VkInstance                                  m_Instance;
        VkSurfaceKHR                                m_Surface;
        VkPhysicalDevice                            m_PhysicalDevice;
        VkDevice                                    m_Device;
        VK::SwapchainResult                         m_SwapchainResult;
        VkExtent2D                                  m_SwapchainExtent2D;
        VkSurfaceFormatKHR                          m_SurfaceFormat;
        VkPresentModeKHR                            m_PresentMode;
        VkCompositeAlphaFlagBitsKHR                 m_SurfaceComposite;
        U32                                         m_ImageCount;
        U32                                         m_CurrentIndex;
        U32                                         m_AcquiredImageIndex;
        VK::Image                                   m_DepthImage;
        VK::ImageView                               m_DepthImageView;
        VkQueue                                     m_GraphicsAndPresentationQueue;
        VkQueue                                     m_ComputeQueue;
        VkQueue                                     m_TransferQueue;
        U32                                         m_GraphicsAndPresentationQueueIndex;
        U32                                         m_ComputeQueueIndex;
        U32                                         m_TransferQueueIndex;
        std::vector<std::pair<VK::QueueType, F32>>  m_RequestedQueues;

    protected:
        RenderContext();
        RenderContext(const RenderContext& context) = default;

    public:
        ~RenderContext() = default;

        static RenderContext* GetInstance() { return s_RenderContext; }

        static void Init(const Window* window, const std::vector<std::pair<VK::QueueType, F32>>& requestedQueues);
        static void Destroy();

        const VkInstance& GetVulkanInstance() const { return m_Instance; }
        const VkSurfaceKHR& GetSurface() const { return m_Surface; }
        const VkPhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
        const VkDevice& GetDevice() const { return m_Device; }
        const VkSwapchainKHR& GetSwapchain() const { return m_SwapchainResult.Swapchain; }
        const VkQueue& GetGraphicsAndPresentationQueue() const { return m_GraphicsAndPresentationQueue; }
        const VkQueue& GetComputeQueue() const { return m_ComputeQueue; }
        const VkQueue& GetTransferQueue() const { return m_TransferQueue; }

        U32 GetGraphicsAndPresentationQueueIndex() const { return m_GraphicsAndPresentationQueueIndex; }
        U32 GetComputeQueueIndex() const { return m_ComputeQueueIndex; }
        U32 GetTransferQueueIndex() const { return m_TransferQueueIndex; }

        VK::SwapchainStatus GetSwapchainStatus() const { return m_SwapchainResult.SwapchainStatus; }
        const VkExtent2D& GetSwapchainExtent2D() const { return m_SwapchainExtent2D; }

        const VkSurfaceFormatKHR& GetSurfaceFormat() const { return m_SurfaceFormat; }
        const VkFormat& GetSwapchainImageFormat() const { return m_SurfaceFormat.format; }

        const std::vector<VK::ImageView>& GetImageViews() const { return m_SwapchainResult.SwapchainImageViews; }
        VK::ImageView GetDepthView() const { return m_DepthImageView; }

        U32 AcquireImageIndex(const VkSemaphore& imageAvailableSemaphore);
        U32 GetCurrentIndex() const { return m_CurrentIndex; }
        U32 GetImageCount() const { return m_ImageCount; }

        void UpdateSwapchain();

        void Present(const VkSemaphore& signalSemaphore);

        void UpdateDepthResources();

    private:
        void InitInternal(const Window* window, const std::vector<std::pair<VK::QueueType, F32>>& requestedQueues);
        void DestroyInternal();

        void CreateInstance();
        void DestroyInstance();

        void CreateSurface();
        void DestroySurface();

        void SelectPhysicalDevice();

        void CreateDevice();
        void DestroyDevice();

        void GetRequestedQueues();

        void CreateSwapchain();
        void DestroySwapchain();

        void CreateDepthResources();
        void DestroyDepthResources();
    };

}
