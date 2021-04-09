#include <BRQ.h>

#include "Renderer.h"

#include "Application/Window.h"
#include "Utilities/VulkanMemoryAllocator.h"

#include "Graphics/Mesh.h"

#include "Platform/Vulkan/RenderContext.h"
#include "Platform/Vulkan/VulkanCommands.h"

#include "Math/Math.h"
#include "Skybox.h"


namespace BRQ {

    // this shouldnt be here
    Mesh mesh;
    Skybox skybox;

    Renderer* Renderer::s_Renderer = nullptr;

    Renderer::Renderer()
        : m_RenderContext(nullptr), m_Window(nullptr) { }

    void Renderer::Init(const Window* window) {

        s_Renderer = new Renderer();
        s_Renderer->InitInternal(window);
    }

    void Renderer::Shutdown() {

        s_Renderer->DestroyInternal();

        delete s_Renderer;
    }

    void Renderer::BeginScene(const Camera& camera) {

        U32 index = m_RenderContext->GetCurrentIndex();

        const PerFrame& perframe = m_PerFrameData[index];

        VK::WaitForFence(m_RenderContext->GetDevice(), perframe.CommandBufferExecutedFence);
        VK::ResetFence(m_RenderContext->GetDevice(), perframe.CommandBufferExecutedFence);

        VK::ResetCommandPool(m_RenderContext->GetDevice(), perframe.CommandPool);

        VkCommandBuffer buffer = perframe.CommandBuffer;

        VkResult result = m_RenderContext->AcquireImageIndex(perframe.ImageAvailableSemaphore);

        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {

            RecreateSwapchain();
            result = m_RenderContext->AcquireImageIndex(perframe.ImageAvailableSemaphore);
        }

        if (result != VK_SUCCESS) {

            VK::QueueWaitIdle(m_RenderContext->GetPresentationQueue());
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
        info.RenderArea.extent = m_RenderContext->GetSwapchainExtent2D();
        info.RenderArea.offset = { 0, 0 };

        VK::CommandBeginRenderPass(info);

        // TEMP
        VkExtent2D extent = m_RenderContext->GetSwapchainExtent2D();
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

        m_Pipeline.Bind(buffer);

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(buffer, 0, 1, &mesh.VertexBuffer.Buffer, &offset);
        vkCmdBindIndexBuffer(buffer, mesh.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);

        glm::mat4 pv = camera.GetProjectionMatrix() * camera.GetViewMatrix();

        m_Pipeline.PushConstantData(buffer, PipelineStage::Vertex, &pv[0], sizeof(glm::mat4), 0);
        m_Pipeline.BindDescriptorSets(buffer, m_PerFrameData[index].DescriptorSets.data(), (U32)m_PerFrameData[index].DescriptorSets.size());

        vkCmdDrawIndexed(buffer, (U32)mesh.IndexCount, 1, 0, 0, 0);

        // ------------------------------------------------------

        m_Skybox.Bind(buffer);
        offset = 0;

        auto vBuffer = skybox.GetVertexBuffer().Buffer;
        auto iBuffer = skybox.GetIndexBuffer().Buffer;

        vkCmdBindVertexBuffers(buffer, 0, 1, &vBuffer, &offset);
        vkCmdBindIndexBuffer(buffer, iBuffer, 0, VK_INDEX_TYPE_UINT32);

        glm::mat4 cam = camera.GetProjectionMatrix() * glm::mat4(glm::mat3(camera.GetViewMatrix()));

        m_Skybox.PushConstantData(buffer, PipelineStage::Vertex, &cam[0], sizeof(glm::mat4), 0);
        m_Skybox.BindDescriptorSets(buffer, m_PerFrameData[index].SkyboxDescriptorSets.data(), (U32)m_PerFrameData[index].SkyboxDescriptorSets.size());

        vkCmdDrawIndexed(buffer, skybox.GetIndexCount(), 1, 0, 0, 0);
    }

    void Renderer::EndScene() {

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
        submitInfo.Queue = m_RenderContext->GetGraphicsQueue();
        submitInfo.CommandBufferExecutedFence = perframe.CommandBufferExecutedFence;

        VK::QueueSubmit(submitInfo);
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
        CreateTexture();
        CreateSkybox();
        
        CreateGraphicsPipeline();
        CreateSkyboxPipeline();

        CreateDescriptorPool();
        CreateDescriptorSets();
        CreateCommands();
        CreateSyncronizationPrimitives();

        //mesh.LoadMesh("Models/monkey_flat.obj");
        mesh.LoadMesh("Resources/Models/Lion.obj");
        //mesh.LoadMesh("Resources/Models/crate.obj");

        skybox.Load();
    }

    void Renderer::DestroyInternal() {

        vkDeviceWaitIdle(m_RenderContext->GetDevice());

        mesh.DestroyMesh();
        skybox.DestroyMesh();

        DestroySyncronizationPrimitives();
        DestroyCommands();
        DestroyGraphicsPipeline();
        DestroySkyboxPipeline();
        DestroyDescriptorPool();
        DestroySkybox();
        DestroyTexture();
        DestroyFramebuffers();
        
        RenderContext::Destroy();
    }

    void Renderer::RecreateSwapchain() {

        VK_CHECK(vkDeviceWaitIdle(m_RenderContext->GetDevice()));

        DestroyFramebuffers();
        m_RenderContext->UpdateSwapchain();
        CreateFramebuffers();
    }

    void Renderer::CreateFramebuffers() {

        m_Framebuffers.resize(m_RenderContext->GetImageCount());

        VkExtent2D extent = m_RenderContext->GetSwapchainExtent2D();

        const auto& views = m_RenderContext->GetImageViews();

        for (U64 i = 0; i < m_Framebuffers.size(); i++) {

            VK::FramebufferCreateInfo info = {};
            info.Attachments = { views[i], m_RenderContext->GetDepthView() };
            info.Layers = 1;
            info.RenderPass = m_RenderContext->GetRenderPass();
            info.Height = extent.height;
            info.Width = extent.width;

            m_Framebuffers[i] = VK::CreateFramebuffer(m_RenderContext->GetDevice(), info);
        }
    }

    void Renderer::DestroyFramebuffers() {

        for (U64 i = 0; i < m_Framebuffers.size(); i++) {

            VK::DestroyFramebuffer(m_RenderContext->GetDevice(), m_Framebuffers[i]);
        }

        m_Framebuffers.clear();
    }

    void Renderer::CreateGraphicsPipeline() {

        BufferLayout layout;
        layout.PushElement(ElementType::Vec3, 3 * sizeof(float));
        layout.PushElement(ElementType::Vec2, 2 * sizeof(float));

        GraphicsPipelineCreateInfo info = {};
        info.Layout = layout;
        info.Flags = (GraphicsPipelineFlags)(EnableCulling | DepthWriteEnabled | DepthTestEnabled | DepthCompareLess);
        info.Shaders = { { "Resources/Shaders/shader.vert.spv" }, { "Resources/Shaders/shader.frag.spv" } };

        m_Pipeline.Init(info);
    }

    void Renderer::DestroyGraphicsPipeline() {

        m_Pipeline.Destroy();
    }

    void Renderer::CreateSkyboxPipeline() {

        BufferLayout layout;
        layout.PushElement(ElementType::Vec3, 3 * sizeof(float));

        GraphicsPipelineCreateInfo info = {};
        info.Layout = layout;
        info.Flags = (GraphicsPipelineFlags)(DepthTestEnabled | DepthCompareLess | DepthCompareEqual | CullModeFrontFace | EnableCulling);
        info.Shaders = { { "Resources/Shaders/skyboxShader.vert.spv" }, { "Resources/Shaders/skyboxShader.frag.spv" } };

        m_Skybox.Init(info);
    }

    void Renderer::DestroySkyboxPipeline() {

        m_Skybox.Destroy();
    }

    void Renderer::CreateCommands() {

        for (U64 i = 0; i < FRAME_LAG; i++) {

            VK::CommandPoolCreateInfo info = {};
            info.Flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            info.QueueFamilyIndex = m_RenderContext->GetGraphicsQueueIndex();

            m_PerFrameData[i].CommandPool = VK::CreateCommandPool(m_RenderContext->GetDevice(), info);

            VK::CommandBufferAllocateInfo allocateInfo = {};
            allocateInfo.CommandPool = m_PerFrameData[i].CommandPool;
            allocateInfo.Level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocateInfo.CommandBufferCount = 1;

            // CHANGE THIS
            auto buffer = VK::AllocateCommandBuffers(m_RenderContext->GetDevice(), allocateInfo);
            m_PerFrameData[i].CommandBuffer = buffer[0];
        }
    }

    void Renderer::DestroyCommands() {

        for (U64 i = 0; i < FRAME_LAG; i++) {

#if defined(BRQ_DEBUG)
            VK::FreeCommandBuffer(m_RenderContext->GetDevice(), m_PerFrameData[i].CommandPool, m_PerFrameData[i].CommandBuffer);
#endif
            VK::DestroyCommandPool(m_RenderContext->GetDevice(), m_PerFrameData[i].CommandPool);
        }
    }

    void Renderer::CreateSyncronizationPrimitives() {

        for (U64 i = 0; i < FRAME_LAG; i++) {

            m_PerFrameData[i].ImageAvailableSemaphore = VK::CreateSemaphore(m_RenderContext->GetDevice());
            m_PerFrameData[i].RenderFinishedSemaphore = VK::CreateSemaphore(m_RenderContext->GetDevice());
            m_PerFrameData[i].CommandBufferExecutedFence = VK::CreateFence(m_RenderContext->GetDevice());
        }
    }

    void Renderer::DestroySyncronizationPrimitives() {

        for (U64 i = 0; i < FRAME_LAG; i++) {

            VK::DestroySemaphore(m_RenderContext->GetDevice(), m_PerFrameData[i].ImageAvailableSemaphore);
            VK::DestroySemaphore(m_RenderContext->GetDevice(), m_PerFrameData[i].RenderFinishedSemaphore);
            VK::DestroyFence(m_RenderContext->GetDevice(), m_PerFrameData[i].CommandBufferExecutedFence);
        }
    }

    void Renderer::CreateDescriptorPool() {

        VkDescriptorPoolSize size = {};
        size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        size.descriptorCount = 1;

        VK::DescriptorPoolCreateInfo info = {};
        info.MaxSets = 4;
        info.PoolSizeCount = 1;
        info.PoolSizes = &size;

        for (U64 i = 0; i < FRAME_LAG; i++) {

            m_PerFrameData[i].SkyboxDescriptorPool = VK::CreateDescriptorPool(m_RenderContext->GetDevice(), info);
            m_PerFrameData[i].DescriptorPool = VK::CreateDescriptorPool(m_RenderContext->GetDevice(), info);
        }
    }

    void Renderer::DestroyDescriptorPool() {

        for (U64 i = 0; i < FRAME_LAG; i++) {

            VK::DestoryDescriptorPool(m_RenderContext->GetDevice(), m_PerFrameData[i].DescriptorPool);
            VK::DestoryDescriptorPool(m_RenderContext->GetDevice(), m_PerFrameData[i].SkyboxDescriptorPool);
        }
    }

    void Renderer::CreateDescriptorSets() {

        std::vector<VkDescriptorSetLayout> layouts = m_Pipeline.GetDescriptorSetLayouts();
        std::vector<VkDescriptorSetLayout> skyboxLayouts =  m_Skybox.GetDescriptorSetLayouts();

        for (U64 i = 0; i < FRAME_LAG; i++) {

            VK::DescriptorSetAllocateInfo info = {};
            info.DescriptorPool = m_PerFrameData[i].DescriptorPool;
            info.DescriptorSetCount = (U32)layouts.size();
            info.SetLayouts = layouts.data();

            m_PerFrameData[i].DescriptorSets = std::move(VK::AllocateDescriptorSets(m_RenderContext->GetDevice(), info));

            info.DescriptorPool = m_PerFrameData[i].SkyboxDescriptorPool;
            info.DescriptorSetCount = (U32)skyboxLayouts.size();
            info.SetLayouts = skyboxLayouts.data();

            m_PerFrameData[i].SkyboxDescriptorSets = std::move(VK::AllocateDescriptorSets(m_RenderContext->GetDevice(), info));

            for (U64 j = 0; j < m_PerFrameData[i].DescriptorSets.size(); j++) {
        
                VkDescriptorImageInfo imageInfo = {};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = m_Texture2D->GetImageView();
                imageInfo.sampler = m_Texture2D->GetSampler();
        
                VkWriteDescriptorSet descriptorWrites = {};
        
                descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites.dstSet = m_PerFrameData[i].DescriptorSets[j];
                descriptorWrites.dstBinding = 0;
                descriptorWrites.dstArrayElement = 0;
                descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites.descriptorCount = 1;
                descriptorWrites.pImageInfo = &imageInfo;
        
                vkUpdateDescriptorSets(m_RenderContext->GetDevice(), 1, &descriptorWrites, 0, nullptr);

                for (size_t j = 0; j < m_PerFrameData[i].SkyboxDescriptorSets.size(); j++) {

                    VkDescriptorImageInfo imageInfo = {};
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfo.imageView = m_TextureCube->GetImageView();
                    imageInfo.sampler = m_TextureCube->GetSampler();

                    VkWriteDescriptorSet descriptorWrites = {};

                    descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrites.dstSet = m_PerFrameData[i].SkyboxDescriptorSets[j];
                    descriptorWrites.dstBinding = 0;
                    descriptorWrites.dstArrayElement = 0;
                    descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    descriptorWrites.descriptorCount = 1;
                    descriptorWrites.pImageInfo = &imageInfo;

                    vkUpdateDescriptorSets(m_RenderContext->GetDevice(), 1, &descriptorWrites, 0, nullptr);
                }
            }
        }
    }

    void Renderer::CreateTexture() {

        m_Texture2D = new Texture2D("Resources/Textures/Lion.jpg");
    }

    void Renderer::DestroyTexture() {

        delete m_Texture2D;
    }

    void Renderer::CreateSkybox() {

        std::vector<std::string_view> filenames = {
            "Resources/Textures/Skybox/posz.jpg",
            "Resources/Textures/Skybox/negz.jpg",
            "Resources/Textures/Skybox/negy.jpg",
            "Resources/Textures/Skybox/posy.jpg",
            "Resources/Textures/Skybox/posx.jpg",
            "Resources/Textures/Skybox/negx.jpg",
        };

        m_TextureCube = new TextureCube(filenames);
    }

    void Renderer::DestroySkybox() {

        delete m_TextureCube;
    }
}