#include <BRQ.h>

#include "VKDevice.h"
#include "VKSemaphore.h"

namespace BRQ {

    VKSemaphore::VKSemaphore()
        : m_Semaphore(VK_NULL_HANDLE), m_Device(nullptr) { }

    void VKSemaphore::Create(const VKDevice* device) {

        m_Device = device;

        VkSemaphoreCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VK_CHECK(vkCreateSemaphore(m_Device->GetLogicalDevice(), &info, nullptr, &m_Semaphore));
    }

    void VKSemaphore::Destroy() {

        if (m_Semaphore) {

            vkDestroySemaphore(m_Device->GetLogicalDevice(), m_Semaphore, nullptr);
            m_Semaphore = VK_NULL_HANDLE;
        }
    }
}