#include <BRQ.h>

#include "VKDevice.h"
#include "VKCommandPool.h"

namespace BRQ {

	VKCommandPool::VKCommandPool()
		: m_CommandPool(VK_NULL_HANDLE), m_Device(nullptr) { }

	std::vector<VKCommandBuffer> VKCommandPool::AllocateCommandBuffers(U64 count) {

		BRQ_CORE_ASSERT(count);

		std::vector<VkCommandBuffer> buffers(count);
		std::vector<VKCommandBuffer> result(count);

		VkCommandBufferAllocateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.commandPool = m_CommandPool;
		info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		info.commandBufferCount = (U32)count;

		VK_CHECK(vkAllocateCommandBuffers(m_Device->GetLogicalDevice(), &info, buffers.data()));

		for (U64 i = 0; i < buffers.size(); i++) {

			result[i].m_CommandBuffer = buffers[i];
		}

		return std::move(result);
	}

	void VKCommandPool::FreeCommandBuffers(std::vector<VKCommandBuffer>& commandBuffers) {

		std::vector<VkCommandBuffer> buffers(commandBuffers.size());

		for (U64 i = 0; i < buffers.size(); i++) {

			buffers[i] = commandBuffers[i].GetCommandBuffer();
		}

		commandBuffers.clear();

		vkFreeCommandBuffers(m_Device->GetLogicalDevice(), m_CommandPool, (U32)buffers.size(), buffers.data());
	}

	void VKCommandPool::Reset() {

		VK_CHECK(vkResetCommandPool(m_Device->GetLogicalDevice(), m_CommandPool, 0));
	}

	void VKCommandPool::Create(const VKDevice* device) {

		m_Device = device;

		VkCommandPoolCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.queueFamilyIndex = m_Device->GetGraphicsFamilyQueueIndex();
		info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VK_CHECK(vkCreateCommandPool(m_Device->GetLogicalDevice(), &info, nullptr, &m_CommandPool));
	}

	void VKCommandPool::Destroy() {

		if (m_CommandPool) {

			vkDestroyCommandPool(m_Device->GetLogicalDevice(), m_CommandPool, nullptr);
			m_CommandPool = VK_NULL_HANDLE;
		}
	}
}