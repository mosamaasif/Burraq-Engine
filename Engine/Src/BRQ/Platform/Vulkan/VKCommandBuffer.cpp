#include <BRQ.h>

#include "VKFence.h"
#include "VKDevice.h"
#include "VKSemaphore.h"
#include "VKCommandPool.h"
#include "VKCommandBuffer.h"

namespace BRQ {

	VKCommandBuffer::VKCommandBuffer()
		: m_CommandBuffer(VK_NULL_HANDLE) { }

	void VKCommandBuffer::Submit(const VKDevice* device, const VKSemaphore* waitSemaphore, const VKSemaphore* signalSemaphore, const VKFence* fence) {

		const std::vector<VkSemaphore> wait = { waitSemaphore->GetSemaphore() };
		const std::vector<VkSemaphore> signal = { signalSemaphore->GetSemaphore() };

		VkPipelineStageFlags stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.pWaitDstStageMask = stages;
		info.pWaitSemaphores = wait.data();
		info.waitSemaphoreCount = (U32)wait.size();
		info.pCommandBuffers = &m_CommandBuffer;
		info.commandBufferCount = 1;
		info.pSignalSemaphores = signal.data();
		info.signalSemaphoreCount = (U32)signal.size();

		VK_CHECK(vkQueueSubmit(device->GetGraphicsFamilyQueue(), 1, &info, fence->GetFence()));
	}

	void VKCommandBuffer::Begin() {

		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VK_CHECK(vkBeginCommandBuffer(m_CommandBuffer, &info));
	}

	void VKCommandBuffer::End() {

		VK_CHECK(vkEndCommandBuffer(m_CommandBuffer));
	}
}