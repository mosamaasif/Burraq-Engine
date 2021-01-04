#pragma once

#include <vulkan/vulkan.h>
#include "VKCommon.h"

namespace BRQ {

    class VKInstance {

    private:
        VkInstance m_VKInstance;

#ifdef BRQ_DEBUG
        VkDebugUtilsMessengerEXT m_DebugMessenger;
#endif

    public:
        VKInstance();
        ~VKInstance() = default;

        void Create();
        void Destroy();

        const VkInstance& GetInstance() const { return m_VKInstance; }
    };
}