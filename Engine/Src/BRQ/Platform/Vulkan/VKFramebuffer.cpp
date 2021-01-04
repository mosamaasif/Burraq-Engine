#include <BRQ.h>

#include "VKDevice.h"
#include "VKSwapChain.h"
#include "VKRenderPass.h"
#include "VKFramebuffer.h"

namespace BRQ {

	VKFramebuffer::VKFramebuffer()
		: m_Framebuffer(VK_NULL_HANDLE), m_Device(nullptr) { }

	//TODO Make class for VkImageView
	void VKFramebuffer::Create(const VKDevice* device, const VKSwapchain* swapchain, const VKRenderPass* renderPass, const VkImageView* imageView) {

		m_Device = device;

		std::vector<VkImageView> attachments = { *imageView };

		VkExtent2D extent = swapchain->GetSwapchainExtent2D();

		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = renderPass->GetRenderPass();
		info.attachmentCount = (U32)attachments.size();
		info.pAttachments = attachments.data();
		info.width = extent.width;
		info.height = extent.height;
		info.layers = 1;

		VK_CHECK(vkCreateFramebuffer(m_Device->GetLogicalDevice(), &info, nullptr, &m_Framebuffer));
	}

	void VKFramebuffer::Destroy() {

		if (m_Framebuffer) {

			vkDestroyFramebuffer(m_Device->GetLogicalDevice(), m_Framebuffer, nullptr);
			m_Framebuffer = VK_NULL_HANDLE;
		}
	}
}
