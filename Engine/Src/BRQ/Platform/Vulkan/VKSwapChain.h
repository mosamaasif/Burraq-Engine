#pragma once

#include <vulkan/vulkan.h>

#include "VKFence.h"
#include "VKCommon.h"
#include "VKSemaphore.h"
#include "VKFramebuffer.h"

#define FRAME_LAG 2

namespace BRQ {

    class VKDevice;
    class VKSurface;
    class VKSemaphore;
    class VKRenderPass;
    class Window;

    class VKSwapchain {

    public:
        enum class SwapchainStatus {

            Ready = 0,
            NotReady
        };

    private:
        VkSwapchainKHR              m_Swapchain;
        const VKSurface*            m_Surface;
        const VKDevice*             m_Device;
        const Window*               m_Window;
        std::vector<VkImage>        m_SwapchainImages;
        std::vector<VkImageView>    m_SwapchainImageViews;
        std::vector<VKFramebuffer>  m_SwapchainFramebuffers;
        U32                         m_CurrentImageIndex;
        U32                         m_AcquiredNextImageIndex;
        VkExtent2D                  m_Dimensions;
        VkSurfaceFormatKHR          m_SurfaceFormat;
        SwapchainStatus             m_SwapchainStatus;

    public:
        VKSwapchain();
        ~VKSwapchain() = default;

        VkFormat GetSurfaceFormat() const { return m_SurfaceFormat.format; }
        const VkExtent2D& GetSwapchainExtent2D() const { return m_Dimensions; }

        SwapchainStatus GetSwapchainStatus() const { return m_SwapchainStatus; }

        const VkSwapchainKHR& GetSwapchain() const { return m_Swapchain; }

        const std::vector<VkImageView>& GetSwapchainImageViews() const { return m_SwapchainImageViews; }
        const std::vector<VkImage>& GetSwapchainImage() const { return m_SwapchainImages; }

        // GetAcquiredNextImageIndex MUST be called after AcquiredNextImageIndex;
        U32 GetAcquiredNextImageIndex() const { return m_AcquiredNextImageIndex; }
        U32 GetCurrentImageIndex() const { return m_CurrentImageIndex; }

        void AcquireNextImageIndex(const VKSemaphore* imageAvailabeSemaphore);

        void Present(const VKSemaphore* signalSemaphore);

        void Create(const VKDevice* device, const VKSurface* surface, const Window* window);
        void Destroy();

        void CreateSwapchainFramebuffers(const VKRenderPass* renderPass);
        void DestroySwapchainFramebuffers();

        const std::vector<VKFramebuffer>& GetSwapchainFramebuffers() const { return m_SwapchainFramebuffers; }

    private:
        VkExtent2D ChooseSwapchainExtent2D() const;
        VkSurfaceFormatKHR ChooseSwapchainFormat() const;
        VkPresentModeKHR ChooseSwapchainPresentMode() const;
    };
}