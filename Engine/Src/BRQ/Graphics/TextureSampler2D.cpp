#include <BRQ.h>
#include "TextureSampler2D.h"

#include "Platform/Vulkan/RenderContext.h"

namespace BRQ {

    TextureSampler2D::TextureSampler2D() {

        Init();
    }

    TextureSampler2D::~TextureSampler2D() {

       auto context = RenderContext::GetInstance();

       VK::DestroySampler(context->GetDevice(), m_TextureSampler);
    }

    void TextureSampler2D::Init() {

        VK::SamplerCreateInfo info = {};
        info.MagFilter = VK_FILTER_LINEAR;
        info.MinFilter = VK_FILTER_LINEAR;
        info.AddressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.AddressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.AddressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        //info.AnisotropyEnable = VK_TRUE;
        //info.MaxAnisotropy = properties.limits.maxSamplerAnisotropy;
        info.BorderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        info.UnnormalizedCoordinates = VK_FALSE;
        info.CompareEnable = VK_FALSE;
        info.CompareOp = VK_COMPARE_OP_ALWAYS;
        info.MipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        auto context = RenderContext::GetInstance();

        m_TextureSampler = VK::CreateSampler(context->GetDevice(), info);
    }
}

