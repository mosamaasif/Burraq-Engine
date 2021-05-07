#pragma once

#include <Engine.h>
#include <BRQ/Application/EntryPoint.h>

namespace MC {

    class Minecraft : public BRQ::Application {

    private:
        BRQ::GraphicsPipeline m_Pipeline;
        BRQ::Material         m_BoxMaterial;
        BRQ::Material         m_LionMaterial;
        BRQ::Texture          m_BoxTexture;
        BRQ::Texture          m_LionTexture;
        BRQ::Mesh             m_BoxMesh;
        BRQ::Mesh             m_LionMesh;
        BRQ::Skybox           m_Skybox;

    public:
        Minecraft() = default;
        Minecraft(const BRQ::WindowProperties& props);

        void OnUpdate(F32 dt) override;

        ~Minecraft();

    private:
        void Init();
    };
}

