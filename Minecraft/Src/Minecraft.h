#pragma once

#include <Engine.h>
#include <BRQ/Application/EntryPoint.h>

namespace MC {

    class Minecraft : public BRQ::Application {

    private:
        BRQ::GraphicsPipeline m_Pipeline;
        BRQ::Texture          m_Texture;
        BRQ::Material         m_Material;
        BRQ::Mesh             m_Mesh;

    public:
        Minecraft() = default;
        Minecraft(const BRQ::WindowProperties& props);

        void OnUpdate(F32 dt) override;

        ~Minecraft();

    private:
        void Init();
    };
}

