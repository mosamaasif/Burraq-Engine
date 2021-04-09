#pragma once

#include "VulkanHelpers.h"

#define FRAME_LAG   3

namespace BRQ {

    class VulkanDevice {

    private:
        const Window*                 m_Window;

        VkInstance                    m_Instance;
        VkSurfaceKHR                  m_Surface;
        VkPhysicalDevice              m_PhysicalDevice;
        VkDevice                      m_Device;
        VkQueue                       m_GraphicsAndPresentationQueue;
        VkQueue                       m_ComputeQueue;
        VkQueue                       m_TransferQueue;
        U32                           m_GraphicsAndPresentationQueueIndex;
        U32                           m_ComputeQueueIndex;
        U32                           m_TransferQueueIndex;

        VkSurfaceFormatKHR            m_SurfaceFormat;
        VkPresentModeKHR              m_SurfacePresentMode;

        VkCompositeAlphaFlagBitsKHR   m_SurfaceComposite;
        VkSurfaceTransformFlagBitsKHR m_SurfaceTransform;
        
        F32                           m_SamplerMaxAnisotropy;
        bool                          m_SamplerAnisotropyEnabled;

        U32                           m_ImageCount;

    public:
        VulkanDevice();
        ~VulkanDevice() = default;

        void Init(const Window* window);
        void Destroy();

        const VkInstance& GetVulkanInstance() const { return m_Instance; }
        const VkSurfaceKHR& GetSurface() const { return m_Surface; }
        const VkPhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
        const VkDevice& GetDevice() const { return m_Device; }

        const VkQueue& GetGraphicsQueue() const { return m_GraphicsAndPresentationQueue; }
        const VkQueue& GetPresentationQueue() const { return m_GraphicsAndPresentationQueue; }
        const VkQueue& GetComputeQueue() const { return m_ComputeQueue; }
        const VkQueue& GetTransferQueue() const { return m_TransferQueue; }

        const U32 GetGraphicsQueueIndex() const { return m_GraphicsAndPresentationQueueIndex; }
        const U32 GetPresentationQueueIndex() const { return m_GraphicsAndPresentationQueueIndex; }
        const U32 GetComputeQueueIndex() const { return m_ComputeQueueIndex; }
        const U32 GetTransferQueueIndex() const { return m_TransferQueueIndex; }

        const VkSurfaceFormatKHR& GetSurfaceFormat() const { return m_SurfaceFormat; }
        const VkPresentModeKHR& GetSurfacePresentMode() const { return m_SurfacePresentMode; }
        const VkCompositeAlphaFlagBitsKHR& GetSurfaceComposite() const { return m_SurfaceComposite; }
        const VkSurfaceTransformFlagBitsKHR& GetSurfaceTransform() const { return m_SurfaceTransform; }

        const VkExtent2D GetSurfaceExtent2D() const { return VK::ChooseSurfaceExtent(m_PhysicalDevice, m_Surface, m_Window); }
        const VkSurfaceCapabilitiesKHR GetSurfaceCapabilities() const { return VK::GetSurfaceCapabilities(m_PhysicalDevice, m_Surface); }

        U32 GetSurfaceImageCount() const { return m_ImageCount; }

    private:
        void CreateVulkanInstance();
        void DestroyVulkanInstance();

        void CreateSurface();
        void DestroySurface();

        void SelectPhysicalDevice();

        void CreateDevice();
        void DestroyDevice();

        void GetQueues();

        void SelectSurfaceFormatAndPresentMode();
        void SelectSurfaceCompositeAndTransform();

        void GetSurfaceSupportedImagesCount();
    };
}