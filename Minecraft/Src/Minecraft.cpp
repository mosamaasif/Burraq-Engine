#include "Minecraft.h"

namespace MC {

    using namespace BRQ;

    Minecraft::Minecraft(const WindowProperties& props)
        : Application(props) {
    
        Init();
    }

    Minecraft::~Minecraft() {
    
        m_Pipeline.Destroy();
        m_Mesh.DestroyMesh();

    }


    void Minecraft::OnUpdate(F32 dt) {

        m_Renderer->Submit({ m_Mesh, m_Material });


    }

    void Minecraft::Init() {

        BufferLayout layout;
        layout.PushElement(ElementType::Vec3);
        layout.PushElement(ElementType::Vec2);

        GraphicsPipelineCreateInfo info = {};
        info.Layout = layout;
        info.Flags = (GraphicsPipelineFlags)(EnableCulling | DepthWriteEnabled | DepthTestEnabled | DepthCompareLess);
        info.Shaders = { { "Resources/Shaders/shader.vert.spv" }, { "Resources/Shaders/shader.frag.spv" } };

        TextureCreateInfo textureInfo = {};
        textureInfo.Type = TextureType::Texture2D;
        textureInfo.Files = { "Resources/Textures/crate.jpg" };
        m_Texture.LoadTexture(textureInfo);

        m_Pipeline.Init(info);

        m_Material.SetPipeline(&m_Pipeline);
        m_Material.SetTexture(&m_Texture);
        m_Material.BuildMaterial();

        m_Mesh.LoadMesh("Resources/Models/crate.obj");
    }
}

BRQ::Application* BRQ::CreateApplication(const BRQ::WindowProperties& props) {

    return new MC::Minecraft(props);
}

