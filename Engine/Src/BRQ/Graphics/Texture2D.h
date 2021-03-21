#pragma once

#include "Platform/Vulkan/VKInitializers.h"

namespace BRQ {

    class Texture2D {

    private:
        VK::Image     m_Image;
        VK::ImageView m_ImageView;

    public:
        Texture2D() = default;
        Texture2D(const std::string_view filename);
        ~Texture2D();

        VK::ImageView GetImageView() const { return m_ImageView; };

        void LoadTexture(const std::string_view filename);
    };
}