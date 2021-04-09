#include <BRQ.h>

#include "BufferLayout.h"

namespace BRQ {

    VkFormat ToVulkanFormat(ElementType type) {

        switch (type) {

        case ElementType::Byte:   return VK_FORMAT_R8_SINT;
        case ElementType::UByte:  return VK_FORMAT_R8_UINT;
        case ElementType::Short:  return VK_FORMAT_R16_SINT;
        case ElementType::UShort: return VK_FORMAT_R16_UINT;
        case ElementType::Int:    return VK_FORMAT_R32_SINT;
        case ElementType::UInt:   return VK_FORMAT_R32_UINT;
        case ElementType::Long:   return VK_FORMAT_R64_SINT;
        case ElementType::ULong:  return VK_FORMAT_R64_UINT;
        case ElementType::Float:  return VK_FORMAT_R32_SFLOAT;
        case ElementType::Double: return VK_FORMAT_R64_SFLOAT;
        case ElementType::Vec2:   return VK_FORMAT_R32G32_SFLOAT;
        case ElementType::Vec3:   return VK_FORMAT_R32G32B32_SFLOAT;
        case ElementType::Vec4:   return VK_FORMAT_R32G32B32A32_SFLOAT;
        }
        return VK_FORMAT_UNDEFINED;
    }

    BufferLayout::BufferLayout() {

        m_Stride = 0;
    }

    void BufferLayout::PushElement(ElementType type, U32 size) {

        BufferElement element;
        element.Type = type;
        element.Size = size;
        element.Offset = (U32)m_Stride;

        m_Elements.push_back(element);

        m_Stride += size;
    }
}

