#include <BRQ.h>
#include "Texture2D.h"

#pragma warning(disable: 6011 26819 6308 28182 6262)  
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma warning(default: 6011 26819 6308 28182 6262)  

#include "Platform/Vulkan/RenderContext.h"

namespace BRQ {

    Texture2D::Texture2D()
        : m_Width(0), m_Height(0), m_Sampler(VK_NULL_HANDLE) { }

    Texture2D::Texture2D(const std::string_view& filename) {

        LoadTexture(filename);
    }

    Texture2D::~Texture2D() {

        auto context = RenderContext::GetInstance();

        VK::DestroyImageView(context->GetDevice(), m_ImageView);
        VK::DestroyImage(m_Image);

        DestroySampler();
    }

    void Texture2D::LoadTexture(const std::string_view& filename) {

        I32 width;
        I32 height;
        I32 channels;

        stbi_set_flip_vertically_on_load(true);

        stbi_uc* image = stbi_load(filename.data(), &width, &height, &channels, STBI_rgb_alpha);

        if (!image) {

            BRQ_CORE_FATAL("Can't load Texture: {}", filename.data());
            return;
        }

        m_Width = width;
        m_Height = height;

        VkDeviceSize size = width * height * 4;

        auto vma = VulkanMemoryAllocator::GetInstance();

        VK::BufferCreateInfo bufferInfo = {};
        bufferInfo.Size = size;
        bufferInfo.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.MemoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;

        VK::Buffer buffer = VK::CreateBuffer(bufferInfo);

        void* data = vma->MapMemory(buffer);

        memcpy_s(data, size, image, size);

        vma->UnMapMemory(buffer);

        VK::ImageCreateInfo imageInfo = {};
        imageInfo.ImageType = VK_IMAGE_TYPE_2D;
        imageInfo.Format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.Extent = { (U32)width, (U32)height, 1U };
        imageInfo.MipLevels = 1;
        imageInfo.ArrayLayers = 1;
        imageInfo.Samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.Tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.Usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

        m_Image = VK::CreateImage(imageInfo);

        auto context = RenderContext::GetInstance();

        VK::CommandPoolCreateInfo poolInfo = {};
        poolInfo.Flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        poolInfo.QueueFamilyIndex = context->GetGraphicsQueueIndex();

        VkCommandPool pool = VK::CreateCommandPool(context->GetDevice(), poolInfo);

        VK::CommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.CommandPool = pool;
        allocateInfo.Level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.CommandBufferCount = 1;

        auto cmd = VK::AllocateCommandBuffers(context->GetDevice(), allocateInfo);

        VK::ImageLayoutTransitionInfo transition = {};
        transition.Image = m_Image.Image;
        transition.Format = VK_FORMAT_R8G8B8A8_UNORM;
        transition.OldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        transition.NewLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        transition.CommandBuffer = cmd[0];
        transition.SubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        transition.SubresourceRange.baseMipLevel = 0;
        transition.SubresourceRange.levelCount = 1;
        transition.SubresourceRange.baseArrayLayer = 0;
        transition.SubresourceRange.layerCount = 1;

        VK::CommandBufferBeginInfo beginInfo = {};
        beginInfo.CommandBuffer = cmd[0];
        beginInfo.Flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK::CommandBufferBegin(beginInfo);

        VK::ImageLayoutTransition(transition);

        VkBufferImageCopy copyInfo = {};
        copyInfo.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyInfo.imageSubresource.layerCount = 1;
        copyInfo.imageOffset = { 0, 0, 0 };
        copyInfo.imageExtent = { m_Width, m_Height, 1U };

        vkCmdCopyBufferToImage(cmd[0], buffer.Buffer, m_Image.Image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);
        
        transition.OldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        transition.NewLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        VK::ImageLayoutTransition(transition);

        VK::CommandBufferEnd(cmd[0]);

        VK::QueueSubmitInfo submitInfo = {};
        submitInfo.CommandBufferCount = 1;
        submitInfo.CommandBuffers = &cmd[0];
        submitInfo.Queue = context->GetGraphicsQueue();

        stbi_image_free(image);

        VK::QueueSubmit(submitInfo);
        VK::QueueWaitIdle(context->GetGraphicsQueue());

        VK::DestroyCommandPool(context->GetDevice(), pool);
        VK::DestoryBuffer(buffer);


        VK::ImageViewCreateInfo viewInfo = {};
        viewInfo.Image = m_Image.Image;
        viewInfo.ViewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.Format = imageInfo.Format;
        viewInfo.SubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.SubresourceRange.baseMipLevel = 0;
        viewInfo.SubresourceRange.levelCount = 1;
        viewInfo.SubresourceRange.baseArrayLayer = 0;
        viewInfo.SubresourceRange.layerCount = 1;

        m_ImageView = VK::CreateImageView(context->GetDevice(), viewInfo);

        CreateSampler();
    }

    void Texture2D::CreateSampler() {

        auto context = RenderContext::GetInstance();

        VkPhysicalDeviceProperties properties = VK::GetPhysicalDeviceProperties(context->GetPhysicalDevice());

        VK::SamplerCreateInfo info = {};
        info.MagFilter = VK_FILTER_LINEAR;
        info.MinFilter = VK_FILTER_LINEAR;
        info.AddressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        info.AddressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        info.AddressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        info.AnisotropyEnable = VK_TRUE;
        info.MaxAnisotropy = properties.limits.maxSamplerAnisotropy;
        info.BorderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        info.UnnormalizedCoordinates = VK_FALSE;
        info.CompareEnable = VK_FALSE;
        info.CompareOp = VK_COMPARE_OP_NEVER;
        info.MipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        m_Sampler = VK::CreateSampler(context->GetDevice(), info);
    }

    void Texture2D::DestroySampler() {

        auto context = RenderContext::GetInstance();

        VK::DestroySampler(context->GetDevice(), m_Sampler);
    }
}