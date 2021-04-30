#pragma once

#include "Events/Event.h"
#include "Camera/Camera.h"
#include "Texture.h"
#include "Platform/Vulkan/RenderContext.h"
#include "GraphicsPipeline.h"

#include "Material.h"
#include "Mesh.h"

#include <queue>

namespace BRQ {

    struct DrawData {

        const Mesh&     Mesh;
        const Material& Material;
    };

    struct SkyboxData {
        
        VkCommandPool                CommandPool;
        VkCommandBuffer              CommandBuffer;
        VkDescriptorPool             SkyboxDescriptorPool;
        std::vector<VkDescriptorSet> SkyboxDescriptorSets;
        bool                         Loaded;
    };

    struct PerFrame {

        VkCommandPool                CommandPool;
        VkCommandBuffer              CommandBuffer;

        VkSemaphore                  ImageAvailableSemaphore;
        VkSemaphore                  RenderFinishedSemaphore;
        VkFence                      CommandBufferExecutedFence;

        VkDescriptorPool             DescriptorPool;
        VkDescriptorPool             SkyboxDescriptorPool;
        std::vector<VkDescriptorSet> DescriptorSets;
        std::vector<VkDescriptorSet> SkyboxDescriptorSets;
    };

    class Renderer {

    private:
        static Renderer*           s_Renderer;
                                   
        const Window*              m_Window;
        RenderContext*             m_RenderContext;
                                   
        Texture*                   m_Texture2D;
        Texture*                   m_TextureCube;
                                   
        GraphicsPipeline           m_Pipeline;
        GraphicsPipeline           m_Skybox;
                                   
        PerFrame                   m_PerFrameData[FRAME_LAG];
        std::vector<VkFramebuffer> m_Framebuffers;

        std::queue<DrawData>       m_RenderQueue;

        Camera                     m_Camera;

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

        void SubmitSkybox();

        void PrepareFrame();
        void EndFrame();

    private:
        void InitInternal(const Window* window);
        void DestroyInternal();

        void RecreateSwapchain();

        void CreateFramebuffers();
        void DestroyFramebuffers();

        void CreateGraphicsPipeline();
        void DestroyGraphicsPipeline();

        void CreateSkyboxPipeline();
        void DestroySkyboxPipeline();

        void CreateCommands();
        void DestroyCommands();

        void CreateSyncronizationPrimitives();
        void DestroySyncronizationPrimitives();

        void CreateDescriptorPool();
        void DestroyDescriptorPool();

        void CreateDescriptorSets();

        // this is temp
        void CreateTexture();
        void DestroyTexture();

        void Present();
    };
}