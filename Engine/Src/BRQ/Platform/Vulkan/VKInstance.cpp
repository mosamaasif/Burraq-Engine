#include <BRQ.h>

#include "VkInstance.h"
#include <vulkan/vulkan_win32.h>

namespace BRQ {

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT              messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT                     messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT*         pCallbackData,
        void* pUserData) {

        switch (messageSeverity) {

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            BRQ_CORE_ERROR(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            BRQ_CORE_WARN(pCallbackData->pMessage);
            break;
        default:
            BRQ_CORE_INFO(pCallbackData->pMessage);
        }

        return VK_FALSE;
    }

    VKInstance::VKInstance()
        : m_VKInstance(VK_NULL_HANDLE) {
    
#ifdef BRQ_DEBUG
        m_DebugMessenger = VK_NULL_HANDLE;
#endif 
    }

    void VKInstance::Create() {

        VkApplicationInfo appInfo = {};

        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "SandBox";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Burraq Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        info.pApplicationInfo = &appInfo;
        
#ifdef BRQ_DEBUG
        const char* layers[] = { "VK_LAYER_KHRONOS_validation" };

        info.ppEnabledLayerNames = layers;
        info.enabledLayerCount = sizeof(layers) / sizeof(layers[0]);
#endif

        const char* extensions[] = {

            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
   
#ifdef BRQ_DEBUG
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
        };

        info.ppEnabledExtensionNames = extensions;
        info.enabledExtensionCount = sizeof(extensions) / sizeof(extensions[0]);

#ifdef BRQ_DEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};

        debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugInfo.pfnUserCallback = debugCallback;

        info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugInfo;
#endif
        VK_CHECK(vkCreateInstance(&info, nullptr, &m_VKInstance));

#ifdef BRQ_DEBUG
        PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VKInstance, "vkCreateDebugUtilsMessengerEXT");
        BRQ_CORE_ASSERT(func);
        VK_CHECK(func(m_VKInstance, &debugInfo, nullptr, &m_DebugMessenger));
#endif 
    }

    void VKInstance::Destroy() {

#ifdef BRQ_DEBUG
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VKInstance, "vkDestroyDebugUtilsMessengerEXT");
        BRQ_CORE_ASSERT(func);
        func(m_VKInstance, m_DebugMessenger, nullptr);
#endif
        if (m_VKInstance) {

            vkDestroyInstance(m_VKInstance, nullptr);
            m_VKInstance = VK_NULL_HANDLE;
        }
    }
}
