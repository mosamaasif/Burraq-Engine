#include <BRQ.h>

#include "Renderer.h"

#include "Application/Window.h"
#include "Utilities/VulkanMemoryAllocator.h"

#include "Graphics/Mesh.h"

#include "Platform/Vulkan/RenderContext.h"
#include "Platform/Vulkan/VulkanCommands.h"

#include "Math/Math.h"
#include "Material.h"

namespace BRQ {

    Renderer* Renderer::s_Renderer = nullptr;

    Renderer::Renderer()
        : m_RenderContext(nullptr), m_Window(nullptr), m_Skybox(nullptr) { }

    void Renderer::Init(const Window* window) {

        s_Renderer = new Renderer();
        s_Renderer->InitInternal(window);
    }

    void Renderer::Shutdown() {

        s_Renderer->DestroyInternal();

        delete s_Renderer;
    }

    void Renderer::Submit(const DrawData data) {

        m_RenderQueue.push(data);
    }

    void Renderer::BeginScene(const Camera& camera) {

        m_Camera = camera;

        PrepareFrame();
    }

    void Renderer::Draw() {

        U32 index = m_RenderContext->GetCurrentIndex();

        const PerFrame& perframe = m_PerFrameData[index];

        VkCommandBuffer buffer = perframe.CommandBuffer;

        VkDeviceSize offset = 0;

        while (!m_RenderQueue.empty()) {

            const DrawData& data = m_RenderQueue.front();

            auto& material = data.Material;

            glm::mat4 pv = m_Camera.GetProjectionMatrix() * m_Camera.GetViewMatrix();

            MaterialBindInfo info = {};
            info.CommandBuffer = buffer;
            info.FrameIndex = index;
            info.PushConstantData = &pv[0];
            info.PushConstantSize = sizeof(glm::mat4);
            info.PushConstantPipelineStage = PipelineStage::Vertex;
            info.PushConstantOffset = 0;

            material.Bind(info);

            auto& vBuffer = data.Mesh.VertexBuffer.Buffer;
            auto& iBuffer = data.Mesh.IndexBuffer.Buffer;

            vkCmdBindVertexBuffers(buffer, 0, 1, &vBuffer, &offset);
            vkCmdBindIndexBuffer(buffer, iBuffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(buffer, (U32)data.Mesh.IndexCount, 1, 0, 0, 0);

            m_RenderQueue.pop();
        }

        glm::mat4 pv = m_Camera.GetProjectionMatrix() * glm::mat4(glm::mat3(m_Camera.GetViewMatrix()));

        MaterialBindInfo info = {};
        info.CommandBuffer = buffer;
        info.FrameIndex = index;
        info.PushConstantData = &pv[0];
        info.PushConstantSize = sizeof(glm::mat4);
        info.PushConstantPipelineStage = PipelineStage::Vertex;
        info.PushConstantOffset = 0;

        m_Skybox->GetMaterial().Bind(info);

        const auto& vBuffer = m_Skybox->GetVertexBuffer().Buffer;
        const auto& iBuffer = m_Skybox->GetIndexBuffer().Buffer;

        vkCmdBindVertexBuffers(buffer, 0, 1, &vBuffer, &offset);
        vkCmdBindIndexBuffer(buffer, iBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(buffer, (U32)m_Skybox->GetIndexCount(), 1, 0, 0, 0);
    }

    void Renderer::EndScene() {

        EndFrame();
        Present();
    }

    void Renderer::SubmitSkybox(const Skybox* skybox) {

        BRQ_ASSERT(skybox);

        m_Skybox = skybox;
    }

    void Renderer::Present() {

        U32 index = m_RenderContext->GetCurrentIndex();

        const PerFrame& perframe = m_PerFrameData[index];

        VkResult result = m_RenderContext->Present(perframe.RenderFinishedSemaphore);

        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {

            RecreateSwapchain();
        }
        else if (result != VK_SUCCESS) {

            BRQ_CORE_ERROR("Failed to present swapchain image.");
        }
    }

    void Renderer::InitInternal(const Window* window) {

        m_Window = window;

        RenderContext::Init(window);

        m_RenderContext = RenderContext::GetInstance();

        CreateFramebuffers();
        CreateCommands();
        CreateSyncronizationPrimitives();
    }

    void Renderer::DestroyInternal() {

        const VulkanDevice& device = m_RenderContext->GetVulkanDevice();
        device.WaitDeviceIdle();

        DestroySyncronizationPrimitives();
        DestroyCommands();
        DestroyFramebuffers();
        
        RenderContext::Destroy();
    }

    void Renderer::RecreateSwapchain() {

        const VulkanDevice device = m_RenderContext->GetVulkanDevice();
        device.WaitDeviceIdle();

        DestroyFramebuffers();
        m_RenderContext->UpdateSwapchain();
        CreateFramebuffers();
    }

    void Renderer::CreateFramebuffers() {

        const VulkanDevice& device = m_RenderContext->GetVulkanDevice();
        const VulkanSwapchain& swapchain= m_RenderContext->GetVulkanSwapchain();

        m_Framebuffers.resize(device.GetSurfaceImageCount());

        VkExtent2D extent = swapchain.GetSwapchainExtent2D();

        const auto& views = swapchain.GetSwapchainImageViews();

        for (U64 i = 0; i < m_Framebuffers.size(); i++) {

            VK::FramebufferCreateInfo info = {};
            info.Attachments = { views[i], m_RenderContext->GetDepthView() };
            info.Layers = 1;
            info.RenderPass = m_RenderContext->GetRenderPass();
            info.Height = extent.height;
            info.Width = extent.width;

            m_Framebuffers[i] = VK::CreateFramebuffer(device.GetDevice(), info);
        }
    }

    void Renderer::DestroyFramebuffers() {

        const VulkanDevice& device = m_RenderContext->GetVulkanDevice();

        for (U64 i = 0; i < m_Framebuffers.size(); i++) {

            VK::DestroyFramebuffer(device.GetDevice(), m_Framebuffers[i]);
        }

        m_Framebuffers.clear();
    }

    void Renderer::CreateCommands() {

        const VulkanDevice& device = m_RenderContext->GetVulkanDevice();

        for (U64 i = 0; i < FRAME_LAG; i++) {

            VK::CommandPoolCreateInfo info = {};
            info.Flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            info.QueueFamilyIndex = device.GetGraphicsQueueIndex();

            m_PerFrameData[i].CommandPool = VK::CreateCommandPool(device.GetDevice(), info);

            VK::CommandBufferAllocateInfo allocateInfo = {};
            allocateInfo.CommandPool = m_PerFrameData[i].CommandPool;
            allocateInfo.Level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocateInfo.CommandBufferCount = 1;

            auto buffer = VK::AllocateCommandBuffers(device.GetDevice(), allocateInfo);
            m_PerFrameData[i].CommandBuffer = buffer[0];
        }
    }

    void Renderer::DestroyCommands() {

        const VulkanDevice& device = m_RenderContext->GetVulkanDevice();

        for (U64 i = 0; i < FRAME_LAG; i++) {

            VK::DestroyCommandPool(device.GetDevice(), m_PerFrameData[i].CommandPool);
        }
    }

    void Renderer::CreateSyncronizationPrimitives() {

        const VulkanDevice& device = m_RenderContext->GetVulkanDevice();

        for (U64 i = 0; i < FRAME_LAG; i++) {

            m_PerFrameData[i].ImageAvailableSemaphore = VK::CreateSemaphore(device.GetDevice());
            m_PerFrameData[i].RenderFinishedSemaphore = VK::CreateSemaphore(device.GetDevice());
            m_PerFrameData[i].CommandBufferExecutedFence = VK::CreateFence(device.GetDevice());
        }
    }

    void Renderer::DestroySyncronizationPrimitives() {

        const VulkanDevice& device = m_RenderContext->GetVulkanDevice();

        for (U64 i = 0; i < FRAME_LAG; i++) {

            VK::DestroySemaphore(device.GetDevice(), m_PerFrameData[i].ImageAvailableSemaphore);
            VK::DestroySemaphore(device.GetDevice(), m_PerFrameData[i].RenderFinishedSemaphore);
            VK::DestroyFence(device.GetDevice(), m_PerFrameData[i].CommandBufferExecutedFence);
        }
    }

    void Renderer::PrepareFrame() {

        const VulkanDevice& device = m_RenderContext->GetVulkanDevice();
        const VulkanSwapchain& swapchain = m_RenderContext->GetVulkanSwapchain();

        U32 index = m_RenderContext->GetCurrentIndex();

        const PerFrame& perframe = m_PerFrameData[index];

        VK::WaitForFence(device.GetDevice(), perframe.CommandBufferExecutedFence);
        VK::ResetFence(device.GetDevice(), perframe.CommandBufferExecutedFence);

        VK::ResetCommandPool(device.GetDevice(), perframe.CommandPool);

        VkCommandBuffer buffer = perframe.CommandBuffer;

        VkResult result = m_RenderContext->AcquireImageIndex(perframe.ImageAvailableSemaphore);

        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {

            RecreateSwapchain();
            result = m_RenderContext->AcquireImageIndex(perframe.ImageAvailableSemaphore);
        }

        if (result != VK_SUCCESS) {

            VK::QueueWaitIdle(device.GetPresentationQueue());
            return;
        }

        U32 imageIndex = m_RenderContext->GetAcquiredImageIndex();

        VK::CommandBufferBeginInfo beginInfo = {};
        beginInfo.CommandBuffer = buffer;
        beginInfo.Flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK::CommandBufferBegin(beginInfo);

        VK::RenderPassBeginInfo info = {};
        info.CommandBuffer = buffer;
        info.RenderPass = m_RenderContext->GetRenderPass();
        info.Framebuffer = m_Framebuffers[imageIndex];
        info.RenderArea.extent = swapchain.GetSwapchainExtent2D();
        info.RenderArea.offset = { 0, 0 };

        VK::CommandBeginRenderPass(info);

        // TEMP
        VkExtent2D extent = swapchain.GetSwapchainExtent2D();
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = (F32)extent.height;
        viewport.width = (F32)extent.width;
        viewport.height = -(F32)extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = extent;

        vkCmdSetViewport(buffer, 0, 1, &viewport);
        vkCmdSetScissor(buffer, 0, 1, &scissor);
    }

    void Renderer::EndFrame() {

        const VulkanDevice& device = m_RenderContext->GetVulkanDevice();

        U32 index = m_RenderContext->GetCurrentIndex();

        const PerFrame& perframe = m_PerFrameData[index];

        VkCommandBuffer buffer = perframe.CommandBuffer;

        VK::CommandEndRenderPass(buffer);
        VK::CommandBufferEnd(buffer);

        VK::QueueSubmitInfo submitInfo = {};
        submitInfo.WaitSemaphoreCount = 1;
        submitInfo.WaitSemaphores = &perframe.ImageAvailableSemaphore;
        submitInfo.WaitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submitInfo.CommandBufferCount = 1;
        submitInfo.CommandBuffers = &buffer;
        submitInfo.SignalSemaphoreCount = 1;
        submitInfo.SignalSemaphores = &perframe.RenderFinishedSemaphore;
        submitInfo.Queue = device.GetGraphicsQueue();
        submitInfo.CommandBufferExecutedFence = perframe.CommandBufferExecutedFence;

        VK::QueueSubmit(submitInfo);
    }
}