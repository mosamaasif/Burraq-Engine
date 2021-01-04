#pragma once

#include <vulkan/vulkan.h>

#include "VKCommon.h"
#include "VKCommandBuffer.h"

namespace BRQ {

	class VKDevice;

	class VKCommandPool {

	private:
		VkCommandPool		m_CommandPool;
		const VKDevice*		m_Device;

	public:
		VKCommandPool();
		~VKCommandPool() = default;

		const VkCommandPool& GetCommandPool() const { return m_CommandPool; }

		std::vector<VKCommandBuffer> AllocateCommandBuffers(U64 count);

		void FreeCommandBuffers(std::vector<VKCommandBuffer>& commandBuffers);

		void Reset();

		void Create(const VKDevice* device);
		void Destroy();
	};
}