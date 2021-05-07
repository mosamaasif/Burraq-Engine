#include "Minecraft.h"

namespace MC {

    using namespace BRQ;

    Minecraft::Minecraft(const WindowProperties& props)
        : Application(props) {
    
        Init();
    }

    Minecraft::~Minecraft() {
    
        m_Pipeline.Destroy();
        m_LionMesh.DestroyMesh();
        m_BoxMesh.DestroyMesh();
    }

    void Minecraft::OnUpdate(F32 dt) {

        m_Renderer->Submit({ m_BoxMesh, m_BoxMaterial });
        m_Renderer->Submit({ m_LionMesh, m_LionMaterial });
    }

    void Minecraft::Init() {

        BufferLayout layout;
        layout.PushElement(ElementType::Vec3);
        layout.PushElement(ElementType::Vec2);

        GraphicsPipelineCreateInfo info = {};
        info.Layout = layout;
        info.Flags = (GraphicsPipelineFlags)(EnableCulling | DepthWriteEnabled | DepthTestEnabled | DepthCompareLess);
        info.Shaders = { { "Resources/Shaders/shader.vert.spv" }, { "Resources/Shaders/shader.frag.spv" } };

        m_Pipeline.Init(info);

        TextureCreateInfo textureInfo = {};
        textureInfo.Type = TextureType::Texture2D;
        textureInfo.Files = { "Resources/Textures/crate.jpg" };
        m_BoxTexture.LoadTexture(textureInfo);

        textureInfo.Type = TextureType::Texture2D;
        textureInfo.Files = { "Resources/Textures/Lion.jpg" };
        m_LionTexture.LoadTexture(textureInfo);

        m_BoxMaterial.SetPipeline(&m_Pipeline);
        m_BoxMaterial.SetTexture(&m_BoxTexture);
        m_BoxMaterial.BuildMaterial();

        m_LionMaterial.SetPipeline(&m_Pipeline);
        m_LionMaterial.SetTexture(&m_LionTexture);
        m_LionMaterial.BuildMaterial();

        m_BoxMesh.LoadMesh("Resources/Models/crate.obj");
        m_LionMesh.LoadMesh("Resources/Models/Lion.obj");

        std::vector<std::string_view> filenames = {
            "Resources/Textures/Skybox/posz.jpg",
            "Resources/Textures/Skybox/negz.jpg",
            "Resources/Textures/Skybox/negy.jpg",
            "Resources/Textures/Skybox/posy.jpg",
            "Resources/Textures/Skybox/posx.jpg",
            "Resources/Textures/Skybox/negx.jpg",
        };

        m_Skybox.Load(filenames);

        m_Renderer->SubmitSkybox(&m_Skybox);
    }
}

BRQ::Application* BRQ::CreateApplication(const BRQ::WindowProperties& props) {

    return new MC::Minecraft(props);
}