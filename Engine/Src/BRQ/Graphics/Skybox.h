#pragma once

#include "Mesh.h"
#include "Texture.h"

#include "GraphicsPipeline.h"
#include "Material.h"

namespace BRQ {

    class Skybox {

    private:
        F32              m_Size;
        Mesh             m_SkyboxMesh;
        Texture          m_CubeMap;
        GraphicsPipeline m_Pipeline;
        Material         m_Material;

    public:
        Skybox();
        Skybox(F32 size);
        ~Skybox();

        void Load(const std::vector<std::string_view>& filenames);
        void DestroyMesh();

        VK::Buffer GetVertexBuffer() const { return m_SkyboxMesh.VertexBuffer; }
        VK::Buffer GetIndexBuffer() const { return m_SkyboxMesh.IndexBuffer; }

        const Material& GetMaterial() const { return m_Material; }

        U32 GetVertexCount() const { return (U32)m_SkyboxMesh.VertexCount; }
        U32 GetIndexCount() const { return (U32)m_SkyboxMesh.IndexCount; }

    private:
        void Init(const std::vector<std::string_view>& filenames);

        void CreatePipeline();
        void DestroyPipline();

        void LoadTexture(const std::vector<std::string_view>& filenames);

        void LoadMesh();

        void CreateMaterial();
    };
}
