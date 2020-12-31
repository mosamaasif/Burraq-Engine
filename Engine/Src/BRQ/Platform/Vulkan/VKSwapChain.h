#pragma once

#include <BRQ.h>

namespace BRQ {

    class VKSurface;
    class VKDevice;
    class Window;

    class VKSwapChain {

    private:
        VkSwapchainKHR              m_SwapChain;
        const VKSurface*            m_Surface;
        const VKDevice*             m_Device;
        const Window*               m_Window;
        std::vector<VkImage>        m_SwapChainImages;
        std::vector<VkImageView>    m_SwapChainImageViews;

    public:
        VKSwapChain();
        ~VKSwapChain() = default;

        void Create(const VKSurface* surface, const VKDevice* device, const Window* window);
        void Destroy();

    private:
        VkExtent2D ChooseSwapChainExtent2D();
        VkSurfaceFormatKHR ChooseSwapChainFormat();
        VkPresentModeKHR ChooseSwapChainPresentMode();
    };
}