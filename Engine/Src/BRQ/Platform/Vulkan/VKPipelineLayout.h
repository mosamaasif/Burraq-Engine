#pragma once

#include <BRQ.h>

#include <vulkan/vulkan.h>

namespace BRQ {

	class VKDevice;

	class VKPipelineLayout {

	private:
		VkPipelineLayout	m_Layout;
		const VKDevice*		m_Device;

	public:
		VKPipelineLayout();
		~VKPipelineLayout() = default;

		VkPipelineLayout GetVulkanPipelineLayout() const { return m_Layout; }

		void Create(const VKDevice* deivce);
		void Destroy();
	};
}