#include <BRQ.h>
#include "TextureCube.h"
#include "Platform/Vulkan/RenderContext.h"

#pragma warning(disable: 6011 26819 6308 28182 6262)  
#include <stb_image.h>
#pragma warning(default: 6011 26819 6308 28182 6262)  

namespace BRQ {

    TextureCube::TextureCube()
        : m_Sampler(VK_NULL_HANDLE) { }

    TextureCube::TextureCube(const std::vector<std::string_view>& filenames) {

        LoadTexture(filenames);
    }

    TextureCube::~TextureCube() {
        
        auto context = RenderContext::GetInstance();

        VK::DestroyImageView(context->GetDevice(), m_ImageView);
        VK::DestroyImage(m_Image);

        DestroySampler();
    }

    void TextureCube::LoadTexture(const std::vector<std::string_view>& filenames) {

        BRQ_ASSERT(filenames.size() == 6);

        std::vector<stbi_uc*> imageData(filenames.size());

        I32 width = 0;
        I32 height = 0;
        I32 channels = 0;

        stbi_set_flip_vertically_on_load(true);

        auto vma = VulkanMemoryAllocator::GetInstance();
        auto context = RenderContext::GetInstance();

        for (U64 i = 0; i < filenames.size(); i++) {

            stbi_uc* image = stbi_load(filenames[i].data(), &width, &height, &channels, STBI_rgb_alpha);

            if (!image) {

                BRQ_CORE_FATAL("Can't load Texture: {}", filenames[i].data());
                return;
            }

            imageData[i] = image;
        }

        VkDeviceSize imageSize = width * height * 4 * filenames.size();
        VkDeviceSize layerSize = imageSize / filenames.size();

        VK::BufferCreateInfo bufferInfo = {};
        bufferInfo.Size = imageSize;
        bufferInfo.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.MemoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;

        VK::Buffer buffer = VK::CreateBuffer(bufferInfo);

        void* data = vma->MapMemory(buffer);

        for (U64 i = 0; i < imageData.size(); i++) {

            U8* mem = (U8*)data + (layerSize * i);
            memcpy_s((void*)mem, layerSize, imageData[i], layerSize);
        }

        vma->UnMapMemory(buffer);        
        
        VK::ImageCreateInfo imageInfo = {};
        imageInfo.ImageType = VK_IMAGE_TYPE_2D;
        imageInfo.Flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        imageInfo.Format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.Extent = { (U32)width, (U32)height, 1U };
        imageInfo.MipLevels = 1;
        imageInfo.ArrayLayers = 6;
        imageInfo.Samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.Tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.Usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

        m_Image = VK::CreateImage(imageInfo);

        VK::CommandPoolCreateInfo poolInfo = {};
        poolInfo.Flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        poolInfo.QueueFamilyIndex = context->GetGraphicsQueueIndex();

        VkCommandPool pool = VK::CreateCommandPool(context->GetDevice(), poolInfo);

        VK::CommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.CommandPool = pool;
        allocateInfo.Level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.CommandBufferCount = 1;

        auto cmd = VK::AllocateCommandBuffers(context->GetDevice(), allocateInfo);

        VK::CommandBufferBeginInfo beginInfo = {};
        beginInfo.CommandBuffer = cmd[0];
        beginInfo.Flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK::CommandBufferBegin(beginInfo);

        std::vector<VkBufferImageCopy> bufferCopyRegions;

        for (U32 face = 0; face < imageData.size(); face++) {

            VkBufferImageCopy bufferCopyRegion = {};
            bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            bufferCopyRegion.imageSubresource.mipLevel = 0;
            bufferCopyRegion.imageSubresource.baseArrayLayer = face;
            bufferCopyRegion.imageSubresource.layerCount = 1;
            bufferCopyRegion.imageExtent.width = width;
            bufferCopyRegion.imageExtent.height = height;
            bufferCopyRegion.imageExtent.depth = 1;
            bufferCopyRegion.bufferOffset = layerSize * face;

            bufferCopyRegions.push_back(bufferCopyRegion);
        }

        VK::ImageLayoutTransitionInfo transition = {};
        transition.Image = m_Image.Image;
        transition.Format = VK_FORMAT_R8G8B8A8_UNORM;
        transition.OldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        transition.NewLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        transition.CommandBuffer = cmd[0];
        transition.SubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        transition.SubresourceRange.baseMipLevel = 0;
        transition.SubresourceRange.levelCount = 1;
        transition.SubresourceRange.layerCount = (U32)imageData.size();

        VK::ImageLayoutTransition(transition);

        vkCmdCopyBufferToImage(cmd[0], buffer.Buffer,
            m_Image.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            (U32)bufferCopyRegions.size(), bufferCopyRegions.data());
 
        transition.OldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        transition.NewLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        VK::ImageLayoutTransition(transition);

        VK::CommandBufferEnd(cmd[0]);

        VK::QueueSubmitInfo submitInfo = {};
        submitInfo.CommandBufferCount = 1;
        submitInfo.CommandBuffers = &cmd[0];
        submitInfo.Queue = context->GetGraphicsQueue();

        VK::QueueSubmit(submitInfo);
        VK::QueueWaitIdle(context->GetGraphicsQueue());

        VK::DestroyCommandPool(context->GetDevice(), pool);
        VK::DestoryBuffer(buffer);

        for (U32 i = 0; i < imageData.size(); i++) {

            stbi_image_free(imageData[i]);
        }

        VK::ImageViewCreateInfo viewInfo = {};
        viewInfo.Image = m_Image.Image;
        viewInfo.ViewType = VK_IMAGE_VIEW_TYPE_CUBE;
        viewInfo.Format = imageInfo.Format;
        viewInfo.SubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.SubresourceRange.baseMipLevel = 0;
        viewInfo.SubresourceRange.levelCount = 1;
        viewInfo.SubresourceRange.baseArrayLayer = 0;
        viewInfo.SubresourceRange.layerCount = 6;

        m_ImageView = VK::CreateImageView(context->GetDevice(), viewInfo);

        CreateSampler();
    }

    void TextureCube::CreateSampler() {

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

    void TextureCube::DestroySampler() {

        auto context = RenderContext::GetInstance();

        VK::DestroySampler(context->GetDevice(), m_Sampler);
    }

}