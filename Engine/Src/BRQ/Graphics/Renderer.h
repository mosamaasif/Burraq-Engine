#pragma once

#include "Events/Event.h"

#include "Platform/Vulkan/VKShader.h"

#include "Platform/Vulkan/RenderContext.h"

namespace BRQ {

    class Renderer {

    private:
        static Renderer*											        s_Renderer;
        static std::vector<std::pair<std::string, VKShader::ShaderType>>    s_ShaderResources;

        const Window*												        m_Window;

        RenderContext*                                                      m_RenderContext;

        VkRenderPass												        m_RenderPass;
        VkPipelineLayout											        m_Layout;
        VkPipeline      											        m_GraphicsPipeline;

        std::vector<VkFramebuffer>                                          m_Framebuffers;

        std::vector<VkCommandPool>                                          m_CommandPools;
        std::vector<VkCommandBuffer>								        m_CommandBuffers;
        std::vector<VkSemaphore>									        m_ImageAvailableSemaphores;
        std::vector<VkSemaphore>									        m_RenderFinishedSemaphores;
        std::vector<VkFence>										        m_CommandBufferExecutedFences;
        std::vector<VKShader>										        m_Shaders;

    protected:
        Renderer();
        Renderer(const Renderer& renderer) = default;

    public:
        ~Renderer() = default;

        static void Init(const Window* window);
        static void Shutdown();

        static Renderer* GetInstance() { return s_Renderer;  }

        static void SubmitResources(const std::vector<std::pair<std::string, VKShader::ShaderType>>& resources) { s_ShaderResources = resources; }

        //void Begin();
        //void End();

        //void BegineScene();
        //void EndScene();

        //void Submit();

        void Present();

    private:
        void InitInternal(const Window* window);
        void DestroyInternal();

        void RecreateSwapchain();

        void LoadShaderResources();
        void DestroyShaderRescources();

        void CreateRenderPass();
        void DestroyRenderPass();

        void CreateFramebuffers();
        void DestroyFramebuffers();

        void CreatePipelineLayout();
        void DestroyPipelineLayout();

        void CreateGraphicsPipeline();
        void DestroyGraphicsPipeline();

        void CreateCommands();
        void DestroyCommands();

        void CreateSyncronizationPrimitives();
        void DestroySyncronizationPrimitives();
    };
}