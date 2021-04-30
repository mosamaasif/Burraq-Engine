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

    void BufferLayout::PushElement(ElementType type) {

        BufferElement element;
        element.Type = type;
        element.Size = GetElementTypeSize(type);
        element.Offset = (U32)m_Stride;

        m_Elements.push_back(element);

        m_Stride += element.Size;
    }

    U32 BufferLayout::GetElementTypeSize(ElementType type) {

        switch (type) {

        case ElementType::Byte:   return sizeof(I8);
        case ElementType::UByte:  return sizeof(U8);
        case ElementType::Short:  return sizeof(I16);
        case ElementType::UShort: return sizeof(U16);
        case ElementType::Int:    return sizeof(I32);
        case ElementType::UInt:   return sizeof(U32);
        case ElementType::Long:   return sizeof(I64);
        case ElementType::ULong:  return sizeof(U64);
        case ElementType::Float:  return sizeof(F32);
        case ElementType::Double: return sizeof(F64);
        case ElementType::Vec2:   return (sizeof(F32) * 2);
        case ElementType::Vec3:   return (sizeof(F32) * 3);
        case ElementType::Vec4:   return (sizeof(F32) * 4);
        }
        return 0;
    }
}

