#include <BRQ.h>

#include "VulkanDevice.h"

namespace BRQ {

    VulkanDevice::VulkanDevice() {

        m_Window = nullptr;
        m_Instance = VK_NULL_HANDLE;
        m_Surface = VK_NULL_HANDLE;
        m_PhysicalDevice = VK_NULL_HANDLE;
        m_Device = VK_NULL_HANDLE;
        m_GraphicsAndPresentationQueue = VK_NULL_HANDLE;
        m_ComputeQueue = VK_NULL_HANDLE;
        m_TransferQueue = VK_NULL_HANDLE;
        m_GraphicsAndPresentationQueueIndex = VK_QUEUE_FAMILY_IGNORED;
        m_ComputeQueueIndex = VK_QUEUE_FAMILY_IGNORED;
        m_TransferQueueIndex = VK_QUEUE_FAMILY_IGNORED;
        m_SurfaceFormat = {};
        m_SurfacePresentMode = VK_PRESENT_MODE_FIFO_KHR;
        m_SurfaceComposite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        m_SurfaceTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        m_SamplerMaxAnisotropy = 0.0f;
        m_SamplerAnisotropyEnabled = false;
        m_ImageCount = 0;
    }

    void VulkanDevice::Init(const Window* window) {

        m_Window = window;

        CreateVulkanInstance();
        CreateSurface();
        SelectPhysicalDevice();
        CreateDevice();
        GetQueues();
    }

    void VulkanDevice::Destroy() {

        DestroyDevice();
        DestroySurface();
        DestroyVulkanInstance();
    }

    void VulkanDevice::CreateVulkanInstance() {

        VK::InstanceCreateInfo info = {};
        info.ApplicationName = m_Window->GetWindowTitle();

        m_Instance = VK::CreateInstance(info);
    }

    void VulkanDevice::DestroyVulkanInstance() {

        VK::DestroyInstance(m_Instance);
    }

    void VulkanDevice::CreateSurface() {

        VK::SurfaceCreateInfo info = {};
        info.InstanceHandle = GetModuleHandle(nullptr);
        info.WindowHandle = (HWND)m_Window->GetNativeWindowHandle();

        m_Surface = VK::CreateSurface(m_Instance, info);
    }

    void VulkanDevice::DestroySurface() {

        VK::DestroySurface(m_Instance, m_Surface);
    }

    void VulkanDevice::SelectPhysicalDevice() {

        m_PhysicalDevice = VK::SeletePhysicalDevice(m_Instance, m_Surface);

        SelectSurfaceFormatAndPresentMode();
        SelectSurfaceCompositeAndTransform();
        GetSurfaceSupportedImagesCount();
    }

    void VulkanDevice::CreateDevice() {

        VkPhysicalDeviceFeatures features = VK::GetPhysicalDeviceFeatures(m_PhysicalDevice);
        m_SamplerAnisotropyEnabled = features.samplerAnisotropy;

        VkPhysicalDeviceFeatures deviceFeatures = {};

        if (m_SamplerAnisotropyEnabled) {

            VkPhysicalDeviceProperties properties = VK::GetPhysicalDeviceProperties(m_PhysicalDevice);
            m_SamplerMaxAnisotropy = properties.limits.maxSamplerAnisotropy;

            deviceFeatures.samplerAnisotropy = VK_TRUE;
        }

        VK::DeviceCreateInfo info = {};
        info.EnabledFeatures = deviceFeatures;

        m_Device = VK::CreateDevice(m_PhysicalDevice, m_Surface, info);
    }

    void VulkanDevice::DestroyDevice() {

        VK::DestroyDevice(m_Device);
    }

    void VulkanDevice::GetQueues() {

        m_GraphicsAndPresentationQueueIndex = VK::GetQueueFamilyIndex(m_PhysicalDevice, m_Surface, VK::QueueType::Graphics);
        m_ComputeQueueIndex = VK::GetQueueFamilyIndex(m_PhysicalDevice, m_Surface, VK::QueueType::AsyncCompute);
        m_TransferQueueIndex = VK::GetQueueFamilyIndex(m_PhysicalDevice, m_Surface, VK::QueueType::AsyncTransfer);
    
        vkGetDeviceQueue(m_Device, m_GraphicsAndPresentationQueueIndex, 0, &m_GraphicsAndPresentationQueue);
        vkGetDeviceQueue(m_Device, m_ComputeQueueIndex, 0, &m_ComputeQueue);
        vkGetDeviceQueue(m_Device, m_TransferQueueIndex, 0, &m_TransferQueue);
    }

    void VulkanDevice::SelectSurfaceFormatAndPresentMode() {

        m_SurfaceFormat = VK::ChooseSurfaceFormat(m_PhysicalDevice, m_Surface);
        m_SurfacePresentMode = VK::ChooseSurfacePresentMode(m_PhysicalDevice, m_Surface);
    }

    void VulkanDevice::SelectSurfaceCompositeAndTransform() {

        VkSurfaceCapabilitiesKHR capabilities = VK::GetSurfaceCapabilities(m_PhysicalDevice, m_Surface);

        if (capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {

            m_SurfaceComposite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        }
        else if (capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {

            m_SurfaceComposite = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
        }
        else if (capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {

            m_SurfaceComposite = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
        }
        else {

            m_SurfaceComposite = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
        }

        m_SurfaceTransform = capabilities.currentTransform;
    }

    void VulkanDevice::GetSurfaceSupportedImagesCount() {

        VkSurfaceCapabilitiesKHR capabilities = VK::GetSurfaceCapabilities(m_PhysicalDevice, m_Surface);

        U32 minLimit = capabilities.minImageCount;
        U32 maxLimit = capabilities.maxImageCount;

        if (FRAME_LAG >= minLimit) {

            if (FRAME_LAG > maxLimit) {

                m_ImageCount = maxLimit;
            }
            else {

                m_ImageCount = FRAME_LAG;
            }
        }
        else {

            m_ImageCount = minLimit;
        }
    }
}
