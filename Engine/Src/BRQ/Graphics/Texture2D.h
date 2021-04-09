#pragma once

#include "Platform/Vulkan/VulkanHelpers.h"

namespace BRQ {

    class Texture2D {

    private:
        VK::Image     m_Image;
        VK::ImageView m_ImageView;
        VkSampler     m_Sampler;
        U32           m_Width;
        U32           m_Height;

    public:
        Texture2D();
        Texture2D(const std::string_view& filename);
        ~Texture2D();

        VK::ImageView GetImageView() const { return m_ImageView; };

        VkSampler GetSampler() const { return m_Sampler; }

        U32 GetTextureWidth() const { return m_Width; }
        U32 GetTextureHeight() const { return m_Height; }

        void LoadTexture(const std::string_view& filename);

    private:
        void CreateSampler();
        void DestroySampler();
    };
}