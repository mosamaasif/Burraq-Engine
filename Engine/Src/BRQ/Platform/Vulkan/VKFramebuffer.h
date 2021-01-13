#pragma once

#include <vulkan/vulkan.h>
#include "VKCommon.h"

namespace BRQ {

    class VKDevice;
    class VKSwapchain;
    class VKRenderPass;

    class VKFramebuffer {

    private:
        VkFramebuffer		m_Framebuffer;
        const VKDevice*		m_Device;

    public:
        VKFramebuffer();
        ~VKFramebuffer() = default;

        const VkFramebuffer& GetFramebuffer() const { return m_Framebuffer; }

        void Create(const VKDevice* device, const VKSwapchain* swapchain, const VKRenderPass* renderPass, const VkImageView* imageView);
        void Destroy();
    };
}