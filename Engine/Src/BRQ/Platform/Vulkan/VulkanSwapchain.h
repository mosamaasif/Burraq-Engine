#pragma once

#include "VulkanHelpers.h"

namespace BRQ {

    class VulkanDevice;

    class VulkanSwapchain {

    private:
        VkSwapchainKHR             m_Swapchain;
        std::vector<VkImage>       m_SwapchainImages;
        std::vector<VK::ImageView> m_SwapchainImageViews;
        VkExtent2D                 m_SwapchainExtent2D;

    public:
        VulkanSwapchain();
        ~VulkanSwapchain() = default;

        void Init(const VulkanDevice& device);
        void Destroy(const VulkanDevice& device);

        void Update(const VulkanDevice& device);

        const VkSwapchainKHR& GetSwapchain() const { return m_Swapchain; }
        const std::vector<VkImage>& GetSwapchainImages() const { return m_SwapchainImages; }
        const std::vector<VK::ImageView>& GetSwapchainImageViews() const { return m_SwapchainImageViews; }
        const VkExtent2D& GetSwapchainExtent2D() const { return m_SwapchainExtent2D; }
    };
}