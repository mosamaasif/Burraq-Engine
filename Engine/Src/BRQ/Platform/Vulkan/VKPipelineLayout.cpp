#include <BRQ.h>

#include "VKDevice.h"
#include "VKPipelineLayout.h"

namespace BRQ {

    VKPipelineLayout::VKPipelineLayout()
        : m_Layout(VK_NULL_HANDLE), m_Device(nullptr) { }

    void VKPipelineLayout::Create(const VKDevice* device) {

        m_Device = device;

        VkPipelineLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.setLayoutCount = 0;
        info.pushConstantRangeCount = 0;

        VK_CHECK(vkCreatePipelineLayout(m_Device->GetLogicalDevice(), &info, nullptr, &m_Layout));
    }

    void VKPipelineLayout::Destroy() {

        if (m_Layout) {

            vkDestroyPipelineLayout(m_Device->GetLogicalDevice(), m_Layout, nullptr);
            m_Layout = VK_NULL_HANDLE;
        }
    }
}