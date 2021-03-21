#pragma once

#include "Platform/Vulkan/VKInitializers.h"

namespace BRQ {

    class TextureSampler2D {

    private:
        VkSampler m_TextureSampler;
        
    public:
        TextureSampler2D();
        ~TextureSampler2D();

        const VkSampler& GetTextureSampler2D() const { return m_TextureSampler; }

    private:
        void Init();
    };
}