#pragma once

#include <BRQ.h>

#include "Platform/Vulkan/VulkanHelpers.h"

namespace BRQ {

    struct Vertex {

        F32 x, y, z;
        F32 u, v;
        //F32 nx, ny, nz;
    };

    struct MeshData {

        std::vector<F32> Verticies;
        std::vector<U32> Indicies;
    };

    struct Mesh {

        VK::Buffer VertexBuffer;
        VK::Buffer IndexBuffer;
        U64        VertexCount;
        U64        IndexCount;

        void LoadMesh(const std::string_view& filename);
        void LoadMesh(const MeshData& meshData);
        void DestroyMesh();
    };
}