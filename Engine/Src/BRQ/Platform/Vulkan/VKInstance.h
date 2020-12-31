#pragma once

#include <BRQ.h>

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

        const VkInstance& GetVulkanInstance() const { return m_VKInstance; }
    };
}