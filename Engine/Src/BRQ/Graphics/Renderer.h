#pragma once

#include "Events/Event.h"
#include "Camera/Camera.h"
#include "Texture2D.h"
#include "TextureCube.h"
#include "Platform/Vulkan/VKShader.h"
#include "Platform/Vulkan/RenderContext.h"

namespace BRQ {

    class Renderer {

    private:
        static Renderer*											s_Renderer;

        const Window*												m_Window;
        RenderContext*                                              m_RenderContext;
        
        Texture2D*                                                  m_Texture2D;
        TextureCube*                                                m_TextureCube;

        VkRenderPass												m_RenderPass;
        VkPipelineLayout											m_Layout;
        VkPipelineLayout                                            m_SkyboxLayout;
        VkPipeline      											m_GraphicsPipeline;
        VkPipeline                                                  m_SkyboxPipeline;

        VkDescriptorSetLayout                                       m_DescriptorSetLayout;
        VkDescriptorSetLayout                                       m_SkyboxDescriptorSetLayout;

        std::vector<VkFramebuffer>                                  m_Framebuffers;
        std::vector<VkDescriptorPool>                               m_DescriptorPool;
        std::vector<VkDescriptorPool>                               m_SkyboxDescriptorPool;

        std::vector<VkDescriptorSet>                                m_DescriptorSet;
        std::vector<VkDescriptorSet>                                m_SkyboxDescriptorSet;

        std::vector<VkCommandPool>                                  m_CommandPools;
        std::vector<VkCommandBuffer>								m_CommandBuffers;
        std::vector<VkSemaphore>									m_ImageAvailableSemaphores;
        std::vector<VkSemaphore>									m_RenderFinishedSemaphores;
        std::vector<VkFence>                                        m_CommandBufferExecutedFences;

        std::vector<std::pair<std::string, VKShader::ShaderType>>   m_ShaderResources;
        std::vector<std::pair<std::string, VKShader::ShaderType>>   m_SkyboxShaderResources;

    protected:
        Renderer();
        Renderer(const Renderer& renderer) = default;

    public:
        ~Renderer() = default;

        static void Init(const Window* window);
        static void Shutdown();

        static Renderer* GetInstance() { return s_Renderer;  }

        void SubmitShaders(const std::vector<std::pair<std::string, VKShader::ShaderType>>& resources);
        void SubmitSkyboxShaders(const std::vector<std::pair<std::string, VKShader::ShaderType>>& resources);

        void BeginScene(const Camera& camera);
        void EndScene();

        //void Submit();

        void Present();

    private:
        void InitInternal(const Window* window);
        void DestroyInternal();

        void RecreateSwapchain();

        void CreateRenderPass();
        void DestroyRenderPass();

        void CreateFramebuffers();
        void DestroyFramebuffers();

        void CreatePipelineLayouts();
        void DestroyPipelineLayouts();

        void CreateGraphicsPipeline();
        void DestroyGraphicsPipeline();

        void CreateSkyboxPipeline();
        void DestroySkyboxPipeline();

        void CreateCommands();
        void DestroyCommands();

        void CreateSyncronizationPrimitives();
        void DestroySyncronizationPrimitives();

        void CreateDescriptorSetLayout();
        void DestoryDescriptorSetLayout();

        void CreateDescriptorPool();
        void DestroyDescriptorPool();

        void CreateDescriptorSets();

        // this is temp
        void CreateTexture();
        void DestroyTexture();

        void CreateSkybox();
        void DestroySkybox();

        std::vector<VKShader> LoadShaders(const std::vector<std::pair<std::string, VKShader::ShaderType>>& resources);
        void DestroyShaders(std::vector<VKShader>& shaders);

        std::vector<VkPipelineShaderStageCreateInfo> GetPipelineShaderStageInfos(const std::vector<VKShader>& shaders);
    };
}