#pragma once

#include <BRQ.h>

#include "Platform/Vulkan/VKInitializers.h"

namespace BRQ {

    struct Vertex {

        F32 x, y, z;
        F32 nx, ny, nz;
        F32 u, v;
    };

    class Mesh {

    private:
        VK::Buffer  m_VertexBuffer;
        VK::Buffer  m_IndexBuffer;
        U64         m_VertexCount;
        U64         m_IndexCount;

    public:
        Mesh();
        Mesh(const std::string_view& filename);
        ~Mesh() = default;

        void LoadMesh(std::string_view filename);

        void DestroyMesh();

        const VkBuffer& GetVertexBuffer() const { return m_VertexBuffer.BufferAllocation.Buffer; }
        const VkBuffer& GetIndexBuffer() const { return m_IndexBuffer.BufferAllocation.Buffer; }
        U64 GetVertexCount() const { return m_VertexCount; }
        U64 GetIndexCount() const { return m_IndexCount; }
    };
}