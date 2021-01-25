#include <BRQ.h>

#include "Mesh.h"

#define FAST_OBJ_IMPLEMENTATION
#include <fast_obj.h>
#include <meshoptimizer.h>

#include "Platform/Vulkan/RenderContext.h"

namespace BRQ {

    Mesh::Mesh()
        : m_VertexBuffer({}), m_IndexBuffer({}), m_VertexCount(0), m_IndexCount(0) { }

    Mesh::Mesh(const std::string_view& filename)
        : m_VertexBuffer({}), m_IndexBuffer({}), m_VertexCount(0), m_IndexCount(0) {

        LoadMesh(filename);
    }

    void Mesh::LoadMesh(std::string_view filename) {

        fastObjMesh* obj = fast_obj_read(filename.data());

        if (!obj) {

            BRQ_CORE_WARN("Failed to Load .obj model! Filename: {}", filename.data());
            return;
        }

        U64 totalIndices = 0;

        for (U32 i = 0; i < obj->face_count; i++)
            totalIndices += 3 * (U64)(obj->face_vertices[i] - 2);

        std::vector<Vertex> vertices(totalIndices);

        U64 vertexOffset = 0;
        U64 indexOffset = 0;

        for (U32 i = 0; i < obj->face_count; ++i) {

            for (U32 j = 0; j < obj->face_vertices[i]; ++j) {

                fastObjIndex gi = obj->indices[indexOffset + j];

                Vertex v;
                v.x =  obj->positions[gi.p * 3 + 0];
                v.y = -obj->positions[gi.p * 3 + 1];
                v.z =  obj->positions[gi.p * 3 + 2];
                v.nx = obj->normals[gi.n * 3 + 0];
                v.ny = obj->normals[gi.n * 3 + 1];
                v.nz = obj->normals[gi.n * 3 + 2];

                if (j >= 3)
                {
                    vertices[vertexOffset + 0] = vertices[vertexOffset - 3];
                    vertices[vertexOffset + 1] = vertices[vertexOffset - 1];
                    vertexOffset += 2;
                }

                vertices[vertexOffset] = v;
                vertexOffset++;
            }

            indexOffset += obj->face_vertices[i];
        }

        fast_obj_destroy(obj);

        std::vector<U32> remap(totalIndices);

        size_t total_vertices = meshopt_generateVertexRemap(&remap[0], NULL, totalIndices, &vertices[0], totalIndices, sizeof(Vertex));

        std::vector<U32> indexBuffer(m_IndexCount = totalIndices);
        meshopt_remapIndexBuffer(&indexBuffer[0], NULL, totalIndices, &remap[0]);

        std::vector<Vertex> vertexBuffer(m_VertexCount = total_vertices);
        meshopt_remapVertexBuffer(&vertexBuffer[0], &vertices[0], totalIndices, sizeof(Vertex), &remap[0]);

        VkDevice device = RenderContext::GetInstance()->GetDevice();
        U32 queueIndex = RenderContext::GetInstance()->GetGraphicsAndPresentationQueueIndex();
        VkQueue queue = RenderContext::GetInstance()->GetGraphicsAndPresentationQueue();

        VK::BufferCreateInfo vertexCreateInfo = {};
        vertexCreateInfo.Size = m_VertexCount * sizeof(Vertex);
        vertexCreateInfo.Flags = 0;
        vertexCreateInfo.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        vertexCreateInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vertexCreateInfo.MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
        vertexCreateInfo.Data = vertexBuffer.data();
        vertexCreateInfo.TransferQueueFamilyIndex = queueIndex;
        vertexCreateInfo.TransferQueue = queue;

        m_VertexBuffer = VK::CreateBuffer(device, vertexCreateInfo);

        VK::BufferCreateInfo indexCreateInfo = {};
        indexCreateInfo.Size = m_IndexCount * sizeof(U32);
        indexCreateInfo.Flags = 0;
        indexCreateInfo.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        indexCreateInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
        indexCreateInfo.MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
        indexCreateInfo.Data = indexBuffer.data();
        indexCreateInfo.TransferQueueFamilyIndex = queueIndex;
        indexCreateInfo.TransferQueue = queue;

        m_IndexBuffer = VK::CreateBuffer(device, indexCreateInfo);
    }

    void Mesh::DestroyMesh() {

        VkDevice device = RenderContext::GetInstance()->GetDevice();

        VK::DestoryBuffer(device, m_VertexBuffer);
        VK::DestoryBuffer(device, m_IndexBuffer);
    }
}