#include <BRQ.h>

#include "VKFence.h"
#include "VKDevice.h"

namespace BRQ {

	VKFence::VKFence()
		: m_Fence(VK_NULL_HANDLE), m_Device(nullptr) { }

	void VKFence::Create(const VKDevice* device) {

		m_Device = device;

		VkFenceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VK_CHECK(vkCreateFence(m_Device->GetLogicalDevice(), &info, nullptr, &m_Fence));
	}

	void VKFence::Destroy() {

		if (m_Fence) {

			vkDestroyFence(m_Device->GetLogicalDevice(), m_Fence, nullptr);
			m_Fence = VK_NULL_HANDLE;
		}
	}

	void VKFence::Wait() {

		if (m_Fence) {

			VK_CHECK(vkWaitForFences(m_Device->GetLogicalDevice(), 1, &m_Fence, VK_TRUE, UINT64_MAX));
		}
	}

	void VKFence::Reset() {

		if (m_Fence) {

			VK_CHECK(vkResetFences(m_Device->GetLogicalDevice(), 1, &m_Fence));
		}
	}

	void VKFence::operator=(const VKFence& fence) {

		m_Fence = fence.m_Fence;
		m_Device = fence.m_Device;
	}
}