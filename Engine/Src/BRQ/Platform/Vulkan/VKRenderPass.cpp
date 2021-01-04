#include <BRQ.h>

#include "VKDevice.h"
#include "VKSwapchain.h"
#include "VKRenderPass.h"
#include "VKFramebuffer.h"
#include "VKCommandBuffer.h"

namespace BRQ {

	VKRenderPass::VKRenderPass()
		: m_RenderPass(VK_NULL_HANDLE), m_Device(nullptr), m_Swapchain(nullptr) { }

	void VKRenderPass::Begin(const VKCommandBuffer* commandBuffer, const VKFramebuffer* framebuffer) {

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.renderPass = m_RenderPass;
		info.framebuffer = framebuffer->GetFramebuffer();
		info.renderArea.offset = { 0, 0 };
		info.renderArea.extent = m_Swapchain->GetSwapchainExtent2D();
		info.clearValueCount = 1;
		info.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer->GetCommandBuffer(), &info, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VKRenderPass::End(const VKCommandBuffer* commandBuffer) {

		vkCmdEndRenderPass(commandBuffer->GetCommandBuffer());
	}

	void VKRenderPass::Create(const VKDevice* device, const VKSwapchain* swapchain) {

		m_Device = device;
		m_Swapchain = swapchain;

		VkAttachmentDescription colorInfo = {};
		colorInfo.format = m_Swapchain->GetSurfaceFormat();
		colorInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		colorInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorReference;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = 1;
		info.pAttachments = &colorInfo;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;
		info.pDependencies = &dependency;
		info.dependencyCount = 1;

		VK_CHECK(vkCreateRenderPass(device->GetLogicalDevice(), &info, nullptr, &m_RenderPass));
	}

	void VKRenderPass::Destroy() {
		
		if (m_RenderPass) {

			vkDestroyRenderPass(m_Device->GetLogicalDevice(), m_RenderPass, nullptr);
			m_RenderPass = VK_NULL_HANDLE;
		}
	}
}