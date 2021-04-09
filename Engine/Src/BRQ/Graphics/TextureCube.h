#pragma once

#include "Platform/Vulkan/VulkanHelpers.h"


namespace BRQ {

    class TextureCube {

    private:
        VK::Image     m_Image;
        VK::ImageView m_ImageView;
        VkSampler     m_Sampler;

    public:
        TextureCube();
        TextureCube(const std::vector<std::string_view>& filenames);
        ~TextureCube();

        VK::ImageView GetImageView() const { return m_ImageView; };
        VkSampler GetSampler() const { return m_Sampler; }

        void LoadTexture(const std::vector<std::string_view>& filenames);

    private:
        void CreateSampler();
        void DestroySampler();
    };
}