#include <BRQ.h>

#include "Mesh.h"

#define FAST_OBJ_IMPLEMENTATION
#include <fast_obj.h>
#include <meshoptimizer.h>

#include "Platform/Vulkan/RenderContext.h"

namespace BRQ {

    void Mesh::LoadMesh(const std::string_view& filename) {

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
                v.y =  obj->positions[gi.p * 3 + 1];
                v.z =  obj->positions[gi.p * 3 + 2];
                v.u =  obj->texcoords[gi.t * 2 + 0];
                v.v =  obj->texcoords[gi.t * 2 + 1];
                //v.nx = obj->normals[gi.n * 3 + 0];
                //v.ny = obj->normals[gi.n * 3 + 1];
                //v.nz = obj->normals[gi.n * 3 + 2];

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

        U64 total_vertices = meshopt_generateVertexRemap(&remap[0], NULL, totalIndices, &vertices[0], totalIndices, sizeof(Vertex));

        MeshData data;
        data.Indicies.resize(totalIndices);
        data.Verticies.resize(total_vertices * sizeof(Vertex));

        meshopt_remapIndexBuffer(&data.Indicies[0], NULL, totalIndices, &remap[0]);
        meshopt_remapVertexBuffer(&data.Verticies[0], &vertices[0], totalIndices, sizeof(Vertex), &remap[0]);

        LoadMesh(data);
    }

    void Mesh::LoadMesh(const MeshData& meshData) {

        VkDevice device = RenderContext::GetInstance()->GetDevice();
        U32 queueIndex = RenderContext::GetInstance()->GetGraphicsAndPresentationQueueIndex();
        VkQueue queue = RenderContext::GetInstance()->GetGraphicsAndPresentationQueue();

        VertexCount = meshData.Verticies.size();
        IndexCount = meshData.Indicies.size();

        VK::BufferCreateInfo vertexCreateInfo = {};
        vertexCreateInfo.Size = VertexCount * sizeof(meshData.Verticies[0]);
        vertexCreateInfo.Flags = 0;
        vertexCreateInfo.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        vertexCreateInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vertexCreateInfo.MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

        VertexBuffer = VK::CreateBuffer(vertexCreateInfo);

        VK::BufferCreateInfo indexCreateInfo = {};
        indexCreateInfo.Size = IndexCount * sizeof(U32);
        indexCreateInfo.Flags = 0;
        indexCreateInfo.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        indexCreateInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
        indexCreateInfo.MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

        IndexBuffer = VK::CreateBuffer(indexCreateInfo);

        VK::CommandPoolCreateInfo poolInfo = {};
        poolInfo.Flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        poolInfo.QueueFamilyIndex = queueIndex;

        VkCommandPool pool = VK::CreateCommandPool(device, poolInfo);

        VK::CommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.CommandPool = pool;
        allocateInfo.Level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.CommandBufferCount = 2;

        auto cmds = VK::AllocateCommandBuffers(device, allocateInfo);

        VK::UploadBufferInfo vertexUpload = {};
        vertexUpload.DestinationBuffer = &VertexBuffer;
        vertexUpload.Data = meshData.Verticies.data();
        vertexUpload.Size = VertexCount * sizeof(meshData.Verticies[0]);
        vertexUpload.Queue = queue;
        vertexUpload.QueueFamilyIndex = queueIndex;
        vertexUpload.CommandBuffer = cmds[0];
        vertexUpload.MemoryUsage = vertexCreateInfo.MemoryUsage;
        vertexUpload.WaitForUpload = true;

        VK::UploadBufferInfo indexUpload = {};
        indexUpload.DestinationBuffer = &IndexBuffer;
        indexUpload.Data = meshData.Indicies.data();
        indexUpload.Size = IndexCount * sizeof(U32);
        indexUpload.Queue = queue;
        indexUpload.QueueFamilyIndex = queueIndex;
        indexUpload.CommandBuffer = cmds[1];
        indexUpload.MemoryUsage = indexCreateInfo.MemoryUsage;
        indexUpload.WaitForUpload = true;

        VK::UploadBuffer(device, vertexUpload);
        VK::UploadBuffer(device, indexUpload);

        VK::FreeCommandBuffer(device, pool, cmds[0]);
        VK::FreeCommandBuffer(device, pool, cmds[1]);

        VK::DestroyCommandPool(device, pool);
    }

    void Mesh::DestroyMesh() {

        VK::DestoryBuffer(VertexBuffer);
        VK::DestoryBuffer(IndexBuffer);
    }
}