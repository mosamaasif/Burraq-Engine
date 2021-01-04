#pragma once

#include <vulkan/vulkan.h>

#include "VKCommon.h"

namespace BRQ {

	class VKDevice;

	class VKFence {

	private:
		VkFence m_Fence;
		const VKDevice* m_Device;

	public:
		VKFence();
		~VKFence() = default;

		const VkFence& GetFence() const { return m_Fence; }

		void Create(const VKDevice* device);
		void Destroy();

		void Wait();
		void Reset();

		void operator = (const VKFence& fence);
	};
}