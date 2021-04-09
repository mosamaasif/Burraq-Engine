#pragma once

#include <BRQ.h>

#include "Logger/Log.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#include "VulkanCommon.h"

#include "Utilities/VulkanMemoryAllocator.h"
#include "Application/Window.h"

#ifdef CreateSemaphore
    #undef CreateSemaphore
#endif

namespace BRQ { namespace VK {

    enum class QueueType {

        Graphics = 0,
        AsyncCompute,
        AsyncTransfer,
    };

#ifdef BRQ_DEBUG

    namespace VKDEBUG {

        static VkDebugUtilsMessengerEXT s_DebugMessenger = VK_NULL_HANDLE;

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT              messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT                     messageTypes,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
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
    }
#endif

    typedef VulkanMemoryAllocator::ImageInfo  Image;
    typedef VulkanMemoryAllocator::BufferInfo Buffer;
    typedef VkImageView                       ImageView;

    BRQ_ALIGN(16) struct InstanceCreateInfo {

        std::string_view ApplicationName = "Vulkan Application";
        std::string_view EngineName = "Burraq Engine";
        U32              ApplicationVersion = VK_MAKE_VERSION(1, 0, 0);
        U32              EngineVersion = VK_MAKE_VERSION(1, 0, 0);
        U32              ApiVersion = VK_API_VERSION_1_2;

        std::vector<const char*> EnabledLayerNames = {};
        std::vector<const char*> EnabledExtensionNames = {};
    };

    BRQ_ALIGN(16) struct SurfaceCreateInfo {

        HWND      WindowHandle = nullptr;
        HINSTANCE InstanceHandle = nullptr;
    };

    BRQ_ALIGN(16) struct DeviceCreateInfo {

        std::vector<const char*> EnabledLayerNames = {};
        std::vector<const char*> EnabledExtensionNames = {};
        VkPhysicalDeviceFeatures EnabledFeatures = {};
    };

    BRQ_ALIGN(16) struct SwapchainCreateInfo {

        VkSurfaceKHR                  Surface = VK_NULL_HANDLE;
        U32                           MinImageCount = 1;
        VkSurfaceFormatKHR            SurfaceFormat = {};
        VkExtent2D                    ImageExtent = { 0, 0 };
        U32                           ImageArrayLayers = 1;
        VkImageUsageFlags             ImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        VkSharingMode                 ImageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        VkSurfaceTransformFlagBitsKHR PreTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        VkCompositeAlphaFlagBitsKHR   CompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        VkPresentModeKHR              PresentMode = VK_PRESENT_MODE_FIFO_KHR;
        VkBool32                      Clipped = VK_TRUE;
        VkSwapchainKHR                OldSwapchain = VK_NULL_HANDLE;
    };

    BRQ_ALIGN(16) struct SwapchainResult {

        VkSwapchainKHR           Swapchain = VK_NULL_HANDLE;
        std::vector<VkImage>     SwapchainImages = {};
        std::vector<ImageView>   SwapchainImageViews = {};
    };

    BRQ_ALIGN(16) struct ImageCreateInfo {

        VkImageCreateFlags    Flags = {};
        VkImageType           ImageType = VK_IMAGE_TYPE_2D;
        VkFormat              Format = VK_FORMAT_UNDEFINED;
        VkExtent3D            Extent = {};
        U32                   MipLevels = 1;
        U32                   ArrayLayers = 1;
        VkSampleCountFlagBits Samples = VK_SAMPLE_COUNT_1_BIT;
        VkImageTiling         Tiling = VK_IMAGE_TILING_OPTIMAL;
        VkImageUsageFlags     Usage = {};
        VkSharingMode         SharingMode = VK_SHARING_MODE_EXCLUSIVE;
        std::vector<U32>      QueueFamilyIndices;
        VkImageLayout         InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VmaMemoryUsage        MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
    };

    BRQ_ALIGN(16) struct ImageViewCreateInfo {

        VkImageViewCreateFlags  Flags = {};
        VkImage                 Image = VK_NULL_HANDLE;
        VkImageViewType         ViewType = VK_IMAGE_VIEW_TYPE_2D;
        VkFormat                Format = VK_FORMAT_UNDEFINED;
        VkComponentMapping      Components = {};
        VkImageSubresourceRange SubresourceRange = {};

    };

    BRQ_ALIGN(16) struct FramebufferCreateInfo {

        VkRenderPass             RenderPass = VK_NULL_HANDLE;
        std::vector<VkImageView> Attachments = {};
        U32                      Width = 0;
        U32                      Height = 0;
        U32                      Layers = 0;
    };

    BRQ_ALIGN(16) struct PresentInfo {

        VkSemaphore    WaitSemaphore = VK_NULL_HANDLE;
        VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
        U32            ImageIndex = ~0U;
        VkQueue        PresentQueue = VK_NULL_HANDLE;
    };
    
    BRQ_ALIGN(16) struct AttachmentDescription {

        VkFormat                     Format = VK_FORMAT_UNDEFINED;
        VkAttachmentDescriptionFlags Flags = {};
        VkSampleCountFlagBits        Samples = VK_SAMPLE_COUNT_1_BIT;
        VkAttachmentLoadOp           LoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        VkAttachmentStoreOp          StoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        VkAttachmentLoadOp           StencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        VkAttachmentStoreOp          StencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        VkImageLayout                InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout                FinalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    };

    BRQ_ALIGN(16) struct RenderPassCreateInfo {

        VkRenderPassCreateFlags              Flags = {};
        std::vector<VkAttachmentDescription> Attachments = {};
        std::vector<VkSubpassDescription>    Subpasses = {};
        std::vector<VkSubpassDependency>     Dependencies = {};
    };

    BRQ_ALIGN(16) struct AttachmentReference {

        U32           Attachment = 0;
        VkImageLayout Layout = VK_IMAGE_LAYOUT_GENERAL;
    };

    BRQ_ALIGN(16) struct SubpassDescription {

        VkPipelineBindPoint                PipelineBindPoint = {};
        VkSubpassDescriptionFlags          Flags = {};
        std::vector<VkAttachmentReference> InputAttachments = {};
        std::vector<VkAttachmentReference> ColorAttachments = {};
        std::vector<VkAttachmentReference> ResolveAttachments = {};
        VkAttachmentReference              DepthStencilAttachment = {};
        std::vector<U32>                   PreserveAttachments = {};
    };

    BRQ_ALIGN(16) struct SubpassDependency {

        U32                  SrcSubpass = VK_SUBPASS_EXTERNAL;
        U32                  DstSubpass = 0;
        VkPipelineStageFlags SrcStageMask = {};
        VkPipelineStageFlags DstStageMask = 0;
        VkAccessFlags        SrcAccessMask = {};
        VkAccessFlags        DstAccessMask = {};
        VkDependencyFlags    DependencyFlags = {};
    };

    BRQ_ALIGN(16) struct RenderPassBeginInfo {

        VkRenderPass    RenderPass = VK_NULL_HANDLE;
        VkFramebuffer   Framebuffer = VK_NULL_HANDLE;
        VkRect2D        RenderArea = {};
        VkClearValue    ColorValues = { 86.0f / 255.0f, 50.0f / 255.0f, 168.0f / 255.0f, 1.0f };
        VkClearValue    DepthValues = { 1.0 };
        VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
    };

    BRQ_ALIGN(16) struct PipelineLayoutCreateInfo {

        std::vector<VkDescriptorSetLayout> SetLayouts;
        std::vector<VkPushConstantRange>   PushConstantRanges;
    };

    BRQ_ALIGN(16) struct GraphicsPipelineCreateInfo {

        VkPipelineCreateFlags                        Flags;
        std::vector<VkPipelineShaderStageCreateInfo> Stages = {};
        VkPipelineVertexInputStateCreateInfo         VertexInputState = {};
        VkPipelineInputAssemblyStateCreateInfo       InputAssemblyState = {};
        VkPipelineTessellationStateCreateInfo        TessellationState = {};
        VkPipelineViewportStateCreateInfo            ViewportState = {};
        VkPipelineRasterizationStateCreateInfo       RasterizationState = {};
        VkPipelineMultisampleStateCreateInfo         MultisampleState = {};
        VkPipelineDepthStencilStateCreateInfo        DepthStencilState = {};
        VkPipelineColorBlendStateCreateInfo          ColorBlendState = {};
        VkPipelineDynamicStateCreateInfo             DynamicState = {};
        VkPipelineLayout                             Layout = VK_NULL_HANDLE;
        VkRenderPass                                 RenderPass = VK_NULL_HANDLE;
        U32                                          Subpass = 0;
        VkPipeline                                   BasePipelineHandle = VK_NULL_HANDLE;
        U32                                          BasePipelineIndex = 0;
    };

    BRQ_ALIGN(16) struct SemaphoreCreateInfo {

        VkSemaphoreCreateFlags Flags = {};
    };

    BRQ_ALIGN(16) struct FenceCreateInfo {

        VkFenceCreateFlags Flags = VK_FENCE_CREATE_SIGNALED_BIT;
    };

    BRQ_ALIGN(16) struct QueueSubmitInfo {
    
        U32                    WaitSemaphoreCount = 0;
        const VkSemaphore*     WaitSemaphores = VK_NULL_HANDLE;
        VkPipelineStageFlags   WaitDstStageMask = {};
        U32                    CommandBufferCount = 0;
        const VkCommandBuffer* CommandBuffers = VK_NULL_HANDLE;
        U32                    SignalSemaphoreCount = 0;
        const VkSemaphore*     SignalSemaphores = VK_NULL_HANDLE;
        VkQueue                Queue = VK_NULL_HANDLE;
        VkFence                CommandBufferExecutedFence = VK_NULL_HANDLE;
    };

    BRQ_ALIGN(16) struct CommandPoolCreateInfo {
    
        VkCommandPoolCreateFlags Flags = {};
        U32                      QueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    };

    BRQ_ALIGN(16) struct CommandBufferAllocateInfo {

        VkCommandPool        CommandPool = VK_NULL_HANDLE;
        VkCommandBufferLevel Level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        U32                  CommandBufferCount = 0;
    };

    BRQ_ALIGN(16) struct CommandBufferBeginInfo {

        VkCommandBuffer                       CommandBuffer = VK_NULL_HANDLE;
        VkCommandBufferUsageFlags             Flags = {};
        const VkCommandBufferInheritanceInfo* InheritanceInfo = nullptr;
    };

    BRQ_ALIGN(16) struct BufferCreateInfo {

        VkBufferCreateFlags      Flags = {};
        VkDeviceSize             Size = 0;
        VkBufferUsageFlags       Usage = {};
        VkSharingMode            SharingMode = {};
        std::vector<U32>         QueueFamilyIndices = {};
        VmaAllocationCreateFlags MemoryFlags = {};
        VmaMemoryUsage           MemoryUsage = {};
    };

    BRQ_ALIGN(16) struct UploadBufferInfo {

        const Buffer*   DestinationBuffer = nullptr;
        const void*     Data = nullptr;
        VkDeviceSize    Size = 0;
        VkQueue         Queue = VK_NULL_HANDLE;
        U32             QueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
        VmaMemoryUsage  MemoryUsage = {};
        bool            WaitForUpload = true;
    };

    BRQ_ALIGN(16) struct DescriptorSetLayoutCreateInfo {

        VkDescriptorSetLayoutCreateFlags    Flags = {};
        U32                                 BindingCount = {};
        const VkDescriptorSetLayoutBinding* Bindings = {};
    };

    BRQ_ALIGN(16) struct DescriptorPoolCreateInfo {

        VkDescriptorPoolCreateFlags Flags = {};
        U32                         MaxSets = {};
        U32                         PoolSizeCount = {};
        const VkDescriptorPoolSize* PoolSizes = {};
    };

    BRQ_ALIGN(16) struct DescriptorSetAllocateInfo {

        VkDescriptorPool             DescriptorPool = {};
        U32                          DescriptorSetCount = {};
        const VkDescriptorSetLayout* SetLayouts = {};
    };

    BRQ_ALIGN(16) struct ImageLayoutTransitionInfo {

        VkImage                 Image = {};
        VkFormat                Format = {};
        VkImageLayout           OldLayout = {};
        VkImageLayout           NewLayout = {};
        VkImageSubresourceRange SubresourceRange = {};
        VkCommandBuffer         CommandBuffer = {};
    };

    BRQ_ALIGN(16) struct SamplerCreateInfo {

        VkSamplerCreateFlags Flags = {};
        VkFilter             MagFilter = {};
        VkFilter             MinFilter = {};
        VkSamplerMipmapMode  MipmapMode = {};
        VkSamplerAddressMode AddressModeU = {};
        VkSamplerAddressMode AddressModeV = {};
        VkSamplerAddressMode AddressModeW = {};
        F32                  MipLodBias = {};
        VkBool32             AnisotropyEnable = {};
        F32                  MaxAnisotropy = {};
        VkBool32             CompareEnable = {};
        VkCompareOp          CompareOp = {};
        F32                  MinLod = {};
        F32                  MaxLod = {};
        VkBorderColor        BorderColor = {};
        VkBool32             UnnormalizedCoordinates = {};
    };

    VkInstance CreateInstance(const InstanceCreateInfo& info = {});
    void DestroyInstance(VkInstance& instance);

    VkSurfaceKHR CreateSurface(const VkInstance& instance, const SurfaceCreateInfo& info = {});
    void DestroySurface(const VkInstance& instance, VkSurfaceKHR& surface);

    VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
    std::vector<VkSurfaceFormatKHR> GetSurfaceFormat(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
    std::vector<VkPresentModeKHR> GetPresentMode(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);

    VkFormat FindSupportedFormat(const VkPhysicalDevice& physicalDevice, const std::vector<VkFormat>& candidates, const VkImageTiling& tiling, const VkFormatFeatureFlags& features);
    VkFormat FindDepthImageFormat(const VkPhysicalDevice& physicalDevice);

    U32 GetQueueFamilyIndex(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, QueueType type);

    VkPhysicalDeviceProperties GetPhysicalDeviceProperties(const VkPhysicalDevice& physicalDevice);
    VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(const VkPhysicalDevice& physicalDevice);

    VkPhysicalDevice SeletePhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface);

    VkDevice CreateDevice(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, const DeviceCreateInfo& info = {});
    void DestroyDevice(VkDevice& device);

    VkSurfaceFormatKHR ChooseSurfaceFormat(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
    VkPresentModeKHR ChooseSurfacePresentMode(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
    VkExtent2D ChooseSurfaceExtent(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, const Window* window);

    // CreateImageView
    ImageView CreateImageView(const VkDevice& device, const ImageViewCreateInfo& info = {});
    void DestroyImageView(const VkDevice& device, ImageView& view);

    Image CreateImage(const ImageCreateInfo& info = {});
    void DestroyImage(Image& image);

    SwapchainResult CreateSwapchain(const VkDevice& device, const SwapchainCreateInfo& info = {});
    SwapchainResult UpdateSwapchain(const VkDevice& device, VkSwapchainKHR& old, const SwapchainCreateInfo& info = {});
    void DestroySwapchain(const VkDevice& device, VkSwapchainKHR& swapchain);

    VkFramebuffer CreateFramebuffer(const VkDevice& device, const FramebufferCreateInfo& info = {});
    void DestroyFramebuffer(const VkDevice& device, VkFramebuffer& framebuffer);

    VkResult Present(const PresentInfo& info = {});

    VkAttachmentDescription GetAttachmentDescription(const AttachmentDescription& description = {});
    VkAttachmentReference GetAttachmentReference(const AttachmentReference& reference = {});

    VkSubpassDescription GetSubpassDescription(const SubpassDescription& description = {});
    VkSubpassDependency GetSubpassDependency(const SubpassDependency& dependency = {});

    VkRenderPass CreateRenderPass(const VkDevice& device, const RenderPassCreateInfo& info = {});
    void DestroyRenderPass(const VkDevice& device, VkRenderPass& renderPass);

    VkPipelineLayout CreatePipelineLayout(const VkDevice& device, const PipelineLayoutCreateInfo& info = {});
    void DestroyPipelineLayout(const VkDevice& device, VkPipelineLayout& layout);
    
    VkPipeline CreateGraphicsPipeline(const VkDevice& device, const GraphicsPipelineCreateInfo info = {});
    void DestroyGraphicsPipeline(const VkDevice& device, VkPipeline& pipeline);

    std::vector<VkPipeline> CreateGraphicsPipelines(const VkDevice& device, const std::vector<GraphicsPipelineCreateInfo>& infos = {});
    void DestroyGraphicsPipelines(const VkDevice& device, std::vector<VkPipeline>& pipelines);

    VkSemaphore CreateSemaphore(const VkDevice device, const SemaphoreCreateInfo& info = {});
    void DestroySemaphore(const VkDevice device, VkSemaphore semaphore);

    VkFence CreateFence(const VkDevice& device, const FenceCreateInfo& info = {});
    void DestroyFence(const VkDevice& device, VkFence& fence);

    void WaitForFence(const VkDevice& device, const VkFence& fence);
    void ResetFence(const VkDevice& device, const VkFence& fence);

    void WaitForFences(const VkDevice& device, const std::vector<VkFence>& fence);
    void ResetFences(const VkDevice& device, const std::vector<VkFence>& fence);

    void QueueSubmit(const QueueSubmitInfo& info = {});
    void QueueWaitIdle(const VkQueue& queue);

    VkCommandPool CreateCommandPool(const VkDevice& device, const CommandPoolCreateInfo& info = {});
    void DestroyCommandPool(const VkDevice& device, VkCommandPool& pool);

    void ResetCommandPool(const VkDevice& device, const VkCommandPool& pool, VkCommandPoolResetFlags resetFlags = {});

    std::vector<VkCommandBuffer> AllocateCommandBuffers(const VkDevice& device, const CommandBufferAllocateInfo& info = {});

    void FreeCommandBuffer(const VkDevice& device, const VkCommandPool& pool, VkCommandBuffer& buffer);

    void CommandBufferBegin(const CommandBufferBeginInfo& info = {});
    void CommandBufferEnd(const VkCommandBuffer& buffer);

    Buffer CreateBuffer(const BufferCreateInfo& info = {});
    void DestoryBuffer(Buffer& buffer);

    void UploadBuffer(const VkDevice& device, const UploadBufferInfo& info = {});

    VkDescriptorSetLayout CreateDescriptorSetLayout(const VkDevice& device, const DescriptorSetLayoutCreateInfo& info = {});
    void DestoryDescriptorSetLayout(const VkDevice& device, VkDescriptorSetLayout& layout);

    VkDescriptorPool CreateDescriptorPool(const VkDevice& device, const DescriptorPoolCreateInfo& info = {});
    void DestoryDescriptorPool(const VkDevice& device, VkDescriptorPool& pool);

    // TODO change this for dynamic descriptor sets
    std::vector<VkDescriptorSet> AllocateDescriptorSets(const VkDevice& device, const DescriptorSetAllocateInfo& info = {});

    void ImageLayoutTransition(const ImageLayoutTransitionInfo info = {});

    VkSampler CreateSampler(const VkDevice& device, const SamplerCreateInfo& info = {});
    void DestroySampler(const VkDevice& device, VkSampler& sampler);

} }