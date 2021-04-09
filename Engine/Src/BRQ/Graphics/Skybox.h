#pragma once

#include "Mesh.h"

namespace BRQ {

    class Skybox {

    private:
        F32  m_Size;
        Mesh m_SkyboxMesh;

    public:
        Skybox();
        Skybox(F32 size);
        ~Skybox() = default;

        void Load();
        void DestroyMesh();

        VK::Buffer GetVertexBuffer() const { return m_SkyboxMesh.VertexBuffer; }
        VK::Buffer GetIndexBuffer() const { return m_SkyboxMesh.IndexBuffer; }

        U32 GetVertexCount() const { return (U32)m_SkyboxMesh.VertexCount; }
        U32 GetIndexCount() const { return (U32)m_SkyboxMesh.IndexCount; }

    private:
        void Init();
    };
}
