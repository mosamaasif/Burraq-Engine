#pragma once

#include <vulkan/vulkan.h>

#include "VKCommon.h"

namespace BRQ {

    class VKDevice;

    class VKSemaphore {

    private:
        VkSemaphore		m_Semaphore;
        const VKDevice* m_Device;

    public:
        VKSemaphore();
        ~VKSemaphore() = default;

        const VkSemaphore& GetSemaphore() const { return m_Semaphore; }

        void Create(const VKDevice* device);
        void Destroy();
    };
}