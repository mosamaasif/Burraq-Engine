#pragma once

#include "Platform/Vulkan/VulkanHelpers.h"

namespace BRQ {

    enum class TextureType {

        Texture2D   = 0,
        TextureCube = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
    };

    struct TextureCreateInfo {

        TextureType                   Type;
        std::vector<std::string_view> Files;
    };

    class Texture {

    private:
        VK::Image     m_Image;
        VK::ImageView m_ImageView;
        VkSampler     m_Sampler;
        U32           m_Width;
        U32           m_Height;
        bool          m_Loaded;

    public:
        Texture();
        Texture(const TextureCreateInfo& info);
        ~Texture();

        VK::ImageView GetImageView() const { return m_ImageView; };
        VkSampler GetSampler() const { return m_Sampler; }

        U32 GetTextureWidth() const { return m_Width; }
        U32 GetTextureHeight() const { return m_Height; }

        void LoadTexture(const TextureCreateInfo& info);
        
    private:
        void CreateSampler();
        void DestroySampler();
    };
}