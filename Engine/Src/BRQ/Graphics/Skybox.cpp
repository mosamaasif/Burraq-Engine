#include <BRQ.h>

#include "Skybox.h"
#include "Mesh.h"

namespace BRQ {

    Skybox::Skybox()
        : m_Size(500.0f) { }

    Skybox::Skybox(F32 size)
        : m_Size(size) { }

    Skybox::~Skybox() {

        m_SkyboxMesh.DestroyMesh();
    }

    void Skybox::Load(const std::vector<std::string_view>& filenames) {

        Init(filenames);
    }

    void Skybox::DestroyMesh() {

        m_SkyboxMesh.DestroyMesh();
    }

    void Skybox::Init(const std::vector<std::string_view>& filenames) {

        CreatePipeline();
        LoadTexture(filenames);
        LoadMesh();
        CreateMaterial();   
    }

    void Skybox::CreatePipeline() {

        BufferLayout layout;
        layout.PushElement(ElementType::Vec3);

        GraphicsPipelineCreateInfo info = {};
        info.Layout = layout;
        info.Flags = (GraphicsPipelineFlags)(DepthTestEnabled | DepthCompareLess | DepthCompareEqual | CullModeFrontFace | EnableCulling);
        info.Shaders = { { "Resources/Shaders/skyboxShader.vert.spv" }, { "Resources/Shaders/skyboxShader.frag.spv" } };

        m_Pipeline.Init(info);
    }

    void Skybox::DestroyPipline() {

        m_Pipeline.Destroy();
    }

    void Skybox::LoadTexture(const std::vector<std::string_view>& filenames) {

        TextureCreateInfo info = {};
        info.Type = TextureType::TextureCube;
        info.Files = filenames;

        m_CubeMap.LoadTexture(info);
    }

    void Skybox::LoadMesh() {

        MeshData meshData;

        meshData.Verticies = {

            -m_Size, -m_Size,  m_Size,
            -m_Size,  m_Size,  m_Size,
             m_Size, -m_Size,  m_Size,
             m_Size,  m_Size,  m_Size,

            -m_Size, -m_Size, -m_Size,
             m_Size, -m_Size, -m_Size,
            -m_Size,  m_Size, -m_Size,
             m_Size,  m_Size, -m_Size,

            -m_Size, -m_Size, -m_Size,
            -m_Size,  m_Size, -m_Size,
            -m_Size, -m_Size,  m_Size,
            -m_Size,  m_Size,  m_Size,

             m_Size, -m_Size,  m_Size,
             m_Size,  m_Size,  m_Size,
             m_Size, -m_Size, -m_Size,
             m_Size,  m_Size, -m_Size,

            -m_Size,  m_Size,  m_Size,
            -m_Size,  m_Size, -m_Size,
             m_Size,  m_Size,  m_Size,
             m_Size,  m_Size, -m_Size,

            -m_Size, -m_Size, -m_Size,
            -m_Size, -m_Size,  m_Size,
             m_Size, -m_Size, -m_Size,
             m_Size, -m_Size,  m_Size
        };

        meshData.Indicies.reserve(6 * 6);

        U32 offset = 0;

        for (U32 i = 0; i < 6 * 6; i++) {

            // CCW { 0, 2, 1, 2, 3, 1 }

            meshData.Indicies.push_back(0 + offset);
            meshData.Indicies.push_back(2 + offset);
            meshData.Indicies.push_back(1 + offset);
            meshData.Indicies.push_back(2 + offset);
            meshData.Indicies.push_back(3 + offset);
            meshData.Indicies.push_back(1 + offset);

            offset += 4;
        }

        m_SkyboxMesh.LoadMesh(meshData);
    }

    void Skybox::CreateMaterial() {

        m_Material.SetPipeline(&m_Pipeline);
        m_Material.SetTexture(&m_CubeMap);
        m_Material.BuildMaterial();
    }
}