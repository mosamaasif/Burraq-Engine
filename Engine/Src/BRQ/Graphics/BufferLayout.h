#pragma once

#include <BRQ.h>

#include "Platform/Vulkan/VulkanHelpers.h"

namespace BRQ {

    enum class ElementType {

        Byte,
        UByte,
        Short,
        UShort,
        Int,
        UInt,
        Long,
        ULong,
        Float,
        Double,
        Vec2,
        Vec3,
        Vec4,
    };

    VkFormat ToVulkanFormat(ElementType type);

    struct BufferElement {

        ElementType Type;
        U32         Size;
        U32         Offset;
    };

    class BufferLayout {

    private:
        U64                        m_Stride;
        std::vector<BufferElement> m_Elements;

    public:
        BufferLayout();
        ~BufferLayout() = default;

        void PushElement(ElementType type, U32 size);

        const std::vector<BufferElement>& GetElements() const { return m_Elements; };
        U64 GetStride() const { return m_Stride; }
    };
}