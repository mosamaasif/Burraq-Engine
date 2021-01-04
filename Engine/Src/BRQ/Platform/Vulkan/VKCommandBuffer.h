#pragma once

#include <vulkan/vulkan.h>

#include "VKCommon.h"

namespace BRQ {

    class VKFence;
    class VKDevice;
    class VKSemaphore;
    class VKCommandPool;

    class VKCommandBuffer {

        friend class VKCommandPool;

    private:
        VkCommandBuffer		m_CommandBuffer;

    public:
        VKCommandBuffer();
        ~VKCommandBuffer() = default;

        const VkCommandBuffer& GetCommandBuffer() const { return m_CommandBuffer; }

        void Submit(const VKDevice* device, const VKSemaphore* waitSemaphore, const VKSemaphore* signalSemaphore, const VKFence* fence);
        
        void Begin();
        void End();
    };
}