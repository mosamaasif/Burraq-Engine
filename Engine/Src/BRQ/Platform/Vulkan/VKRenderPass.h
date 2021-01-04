#pragma once

#include <vulkan/vulkan.h>

namespace BRQ {

	class VKDevice;
	class VKSwapchain;
	class VKFramebuffer;
	class VKCommandBuffer;

	class VKRenderPass {

	private:
		VkRenderPass		m_RenderPass;
		const VKDevice*		m_Device;
		const VKSwapchain*	m_Swapchain;

	public:
		VKRenderPass();
		~VKRenderPass() = default;

		const VkRenderPass& GetRenderPass() const { return m_RenderPass; }

		void Begin(const VKCommandBuffer* commandBuffer, const VKFramebuffer* framebuffer);
		void End(const VKCommandBuffer* commandBuffer);

		void Create(const VKDevice* device, const VKSwapchain* swapchain);
		void Destroy();
	};
}
