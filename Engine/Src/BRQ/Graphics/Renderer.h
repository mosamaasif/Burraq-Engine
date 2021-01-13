#pragma once

#include "Events/Event.h"

#include "Platform/Vulkan/VKFence.h"
#include "Platform/Vulkan/VKDevice.h"
#include "Platform/Vulkan/VKShader.h"
#include "Platform/Vulkan/VKSurface.h"
#include "Platform/Vulkan/VKInstance.h"
#include "Platform/Vulkan/VKSwapchain.h"
#include "Platform/Vulkan/VKSemaphore.h"
#include "Platform/Vulkan/VKRenderPass.h"
#include "Platform/Vulkan/VKCommandPool.h"
#include "Platform/Vulkan/VKCommandBuffer.h"
#include "Platform/Vulkan/VKPipelineLayout.h"
#include "Platform/Vulkan/VKGraphicsPipeline.h"

namespace BRQ {

    class Renderer {

    private:
        static Renderer*											        s_Renderer;
        static std::vector<std::pair<std::string, VKShader::ShaderType>>    s_ShaderResources;

        VKInstance*													        m_VulkanInstance;
        VKSurface*													        m_Surface;
        VKDevice*													        m_Device;
        VKSwapchain*												        m_Swapchain;
        VKRenderPass*												        m_RenderPass;
        VKPipelineLayout*											        m_Layout;
        VKGraphicsPipeline*											        m_GraphicsPipeline;
        const Window*												        m_Window;

        std::vector<VKCommandPool>                                          m_CommandPools;
        std::vector<VKCommandBuffer>								        m_CommandBuffers;
        std::vector<VKSemaphore>									        m_ImageAvailableSemaphores;
        std::vector<VKSemaphore>									        m_RenderFinishedSemaphores;
        std::vector<VKFence>										        m_CommandBufferExecutedFences;
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

        void Begin();
        void End();

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

        void CreateInstance();
        void DestroyInstance();

        void CreateSurface();
        void DestroySurface();

        void CreateDevice();
        void DestroyDevice();

        void CreateSwapchain();
        void DestroySwapchain();

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