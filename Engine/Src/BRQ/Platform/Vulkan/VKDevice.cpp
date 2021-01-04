#include <BRQ.h>

#include "VKDevice.h"
#include "VKSurface.h"
#include "VKInstance.h"

namespace BRQ {

    VKDevice::VKDevice()
        : m_PhysicalDevice(VK_NULL_HANDLE), m_LogicalDevice(VK_NULL_HANDLE),
          m_GraphicsQueue(VK_NULL_HANDLE), m_PresentationQueue(VK_NULL_HANDLE),
          m_GraphicsQueueIndex(UINT32_MAX), m_PresentationQueueIndex(UINT32_MAX) { }

    void VKDevice::Create(const VKInstance* vkInstance, const VKSurface* surface) {

        SelectPhysicalDevice(&vkInstance->GetInstance(), &surface->GetSurface());
        CreateLogicalDevice(&surface->GetSurface());
    }

    void VKDevice::Destroy() {

        if (m_LogicalDevice) {

            vkDestroyDevice(m_LogicalDevice, nullptr);
            m_LogicalDevice = VK_NULL_HANDLE;
        }
    }

    void VKDevice::SelectPhysicalDevice(const VkInstance* vkInstance, const VkSurfaceKHR* surface) {

        U32 deviceCount = 0;

        VK_CHECK(vkEnumeratePhysicalDevices(*vkInstance, &deviceCount, nullptr));
        BRQ_CORE_ASSERT(deviceCount);

        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);

        VK_CHECK(vkEnumeratePhysicalDevices(*vkInstance, &deviceCount, &physicalDevices[0]));

        for (const auto& device : physicalDevices) {

            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(device, &props);

            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(device, &features);

            VkPhysicalDeviceMemoryProperties memProps;
            vkGetPhysicalDeviceMemoryProperties(device, &memProps);

            U32 presentation = GetQueueFamilyIndex(&device, QueueType::Presentation, surface);
            U32 graphics = GetQueueFamilyIndex(&device, QueueType::Graphics, surface);

            if ((presentation == VK_QUEUE_FAMILY_IGNORED) && (graphics == VK_QUEUE_FAMILY_IGNORED))
                continue;

            if (props.apiVersion < VK_API_VERSION_1_2)
                continue;

            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {

                m_PhysicalDevice = device;

                m_GraphicsQueueIndex = graphics;
                m_PresentationQueueIndex = presentation;
                break;
            }
        }

        if (m_PhysicalDevice) {

            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(m_PhysicalDevice, &props);

            BRQ_CORE_TRACE("Renderer: {}", props.deviceName);
        }
        else {

            BRQ_CORE_FATAL("Can't Select Discrete GPU!");
            BRQ_CORE_ASSERT(false);
        }
    }

    void VKDevice::CreateLogicalDevice(const VkSurfaceKHR* surface) {

        const F32 queuePriority[] = { 1.0f };

        U32 presentation = GetQueueFamilyIndex(&m_PhysicalDevice, QueueType::Presentation, surface);
        U32 graphics = GetQueueFamilyIndex(&m_PhysicalDevice, QueueType::Graphics, surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<U32> queueFamilies = { presentation, graphics };

        for (U32 queueFamily : queueFamilies) {
            VkDeviceQueueCreateInfo info = {};
           info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
           info.queueFamilyIndex = queueFamily;
           info.queueCount = 1;
           info.pQueuePriorities = queuePriority;
           queueCreateInfos.push_back(info);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo info = {};

        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        info.queueCreateInfoCount = (U32)queueCreateInfos.size();
        info.pQueueCreateInfos = queueCreateInfos.data();
        info.pEnabledFeatures = &deviceFeatures;
        info.enabledExtensionCount = 0;
        info.enabledLayerCount = 0;

#ifdef BRQ_DEBUG
        const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
        info.ppEnabledLayerNames = layers;
        info.enabledLayerCount = sizeof(layers) / sizeof(layers[0]);
#endif
        const char* extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        info.ppEnabledExtensionNames = extensions;
        info.enabledExtensionCount = sizeof(extensions) / sizeof(extensions[0]);

        VK_CHECK(vkCreateDevice(m_PhysicalDevice, &info, nullptr, &m_LogicalDevice));

        vkGetDeviceQueue(m_LogicalDevice, graphics, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_LogicalDevice, presentation, 0, &m_PresentationQueue);
    }

    U32 VKDevice::GetQueueFamilyIndex(const VkPhysicalDevice* device, QueueType type, const VkSurfaceKHR* surface) const {

        U32 queueCount = 0;

        vkGetPhysicalDeviceQueueFamilyProperties(*device, &queueCount, nullptr);

        std::vector<VkQueueFamilyProperties> queues(queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(*device, &queueCount, &queues[0]);

        for (U32 i = 0; i < queueCount; i++) {

            if (type == QueueType::Graphics) {

                if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                
                    return i;
                }
            }
            else if (type == QueueType::Presentation) {

                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(*device, i, *surface, &presentSupport);

                if (presentSupport) {

                    return i;
                }
            }
            else if (type == QueueType::Compute) {
            
                if (queues[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {

                    return i;
                }
            }
        }
        return VK_QUEUE_FAMILY_IGNORED;
    }
}