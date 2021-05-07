#pragma once

#include "Events/Event.h"
#include "Camera/Camera.h"
#include "Texture.h"
#include "Platform/Vulkan/RenderContext.h"
#include "GraphicsPipeline.h"

#include "Material.h"
#include "Mesh.h"
#include "Skybox.h"
#include <queue>

namespace BRQ {

    struct DrawData {

        const Mesh&     Mesh;
        const Material& Material;
    };

    struct PerFrame {

        VkCommandPool                CommandPool;
        VkCommandBuffer              CommandBuffer;

        VkSemaphore                  ImageAvailableSemaphore;
        VkSemaphore                  RenderFinishedSemaphore;
        VkFence                      CommandBufferExecutedFence;
    };

    class Renderer {

    private:
        static Renderer*           s_Renderer;
                                   
        const Window*              m_Window;
        RenderContext*             m_RenderContext;
                                   
        PerFrame                   m_PerFrameData[FRAME_LAG];
        std::vector<VkFramebuffer> m_Framebuffers;

        // make this std::vector
        std::queue<DrawData>       m_RenderQueue;
        Camera                     m_Camera;

        const Skybox*              m_Skybox;

    protected:
        Renderer();
        Renderer(const Renderer& renderer) = default;

    public:
        ~Renderer() = default;

        static void Init(const Window* window);
        static void Shutdown();

        static Renderer* GetInstance() { return s_Renderer;  }

        void Submit(DrawData data);

        void BeginScene(const Camera& camera);
        void Draw();
        void EndScene();

        void SubmitSkybox(const Skybox* skybox);

        void PrepareFrame();
        void EndFrame();

    private:
        void InitInternal(const Window* window);
        void DestroyInternal();

        void RecreateSwapchain();

        void CreateFramebuffers();
        void DestroyFramebuffers();

        void CreateCommands();
        void DestroyCommands();

        void CreateSyncronizationPrimitives();
        void DestroySyncronizationPrimitives();

        void Present();
    };
}