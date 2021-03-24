#include <BRQ.h>

#include "Renderer.h"

#include "Application/Window.h"
#include "Utilities/VulkanMemoryAllocator.h"

#include "Graphics/Mesh.h"

#include "Platform/Vulkan/RenderContext.h"
#include "Platform/Vulkan/VKCommands.h"

#include "Math/Math.h"

namespace BRQ {

    // this shouldnt be here
    Mesh mesh;
    Renderer* Renderer::s_Renderer = nullptr;

    Renderer::Renderer()
        : m_RenderContext(nullptr), m_RenderPass(nullptr),
        m_Layout(nullptr), m_GraphicsPipeline(nullptr), m_Window(nullptr) { }

    void Renderer::Init(const Window* window) {

        s_Renderer = new Renderer();

        s_Renderer->InitInternal(window);
    }

    void Renderer::Shutdown() {

        s_Renderer->DestroyInternal();

        delete s_Renderer;
    }

    void Renderer::SubmitResources(const std::vector<std::pair<std::string, VKShader::ShaderType>>& resources) {

        m_ShaderResources = resources;

        CreateGraphicsPipeline();
    }

    void Renderer::BeginScene(const Camera& camera) {

        U32 index = m_RenderContext->GetCurrentIndex();

        VK::WaitForFence(m_RenderContext->GetDevice(), m_CommandBufferExecutedFences[index]);
        VK::ResetFence(m_RenderContext->GetDevice(), m_CommandBufferExecutedFences[index]);

        VK::ResetCommandPool(m_RenderContext->GetDevice(), m_CommandPools[index]);

        VkCommandBuffer buffer = m_CommandBuffers[index];

        VkResult result = m_RenderContext->AcquireImageIndex(m_ImageAvailableSemaphores[index]);

        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {

            RecreateSwapchain();
            result = m_RenderContext->AcquireImageIndex(m_ImageAvailableSemaphores[index]);
        }

        if (result != VK_SUCCESS) {

            VK::QueueWaitIdle(m_RenderContext->GetGraphicsAndPresentationQueue());
            return;
        }

        U32 imageIndex = m_RenderContext->GetAcquiredImageIndex();

        VK::CommandBufferBeginInfo beginInfo = {};
        beginInfo.CommandBuffer = buffer;
        beginInfo.Flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK::CommandBufferBegin(beginInfo);

        VK::RenderPassBeginInfo info = {};
        info.CommandBuffer = buffer;
        info.RenderPass = m_RenderPass;
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

        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(buffer, 0, 1, &mesh.GetVertexBuffer(), &offset);
        vkCmdBindIndexBuffer(buffer, mesh.GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

        glm::mat4 pv = camera.GetProjectionMatrix() * camera.GetViewMatrix();

        vkCmdPushConstants(buffer, m_Layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &pv[0]);

        vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Layout, 0, 1, &m_DescriptorSet[index], 0, nullptr);

        vkCmdDrawIndexed(buffer, (U32)mesh.GetIndexCount(), 1, 0, 0, 0);
    }

    void Renderer::EndScene() {

        U32 index = m_RenderContext->GetCurrentIndex();
        VkCommandBuffer buffer = m_CommandBuffers[index];

        VK::CommandEndRenderPass(buffer);

        VK::CommandBufferEnd(buffer);

        VK::QueueSubmitInfo submitInfo = {};
        submitInfo.WaitSemaphoreCount = 1;
        submitInfo.WaitSemaphores = &m_ImageAvailableSemaphores[index];
        submitInfo.WaitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submitInfo.CommandBufferCount = 1;
        submitInfo.CommandBuffers = &buffer;
        submitInfo.SignalSemaphoreCount = 1;
        submitInfo.SignalSemaphores = &m_RenderFinishedSemaphores[index];
        submitInfo.Queue = m_RenderContext->GetGraphicsAndPresentationQueue();
        submitInfo.CommandBufferExecutedFence = m_CommandBufferExecutedFences[index];

        VK::QueueSubmit(submitInfo);
    }

    void Renderer::Present() {

        U32 index = m_RenderContext->GetCurrentIndex();
        VkResult result = m_RenderContext->Present(m_RenderFinishedSemaphores[index]);

        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {

            RecreateSwapchain();
        }
        else if (result != VK_SUCCESS) {

            BRQ_CORE_ERROR("Failed to present swapchain image.");
        }
    }

    void Renderer::InitInternal(const Window* window) {

        m_Window = window;

        RenderContext::Init(window, { { VK::QueueType::Graphics, 1.0f } });

        m_RenderContext = RenderContext::GetInstance();

        CreateRenderPass();
        CreateFramebuffers();
        CreateDescriptorSetLayout();
        CreateTexture();
        CreateTextureSampler2D();
        CreateDescriptorPool();
        CreateDescriptorSets();
        CreatePipelineLayout();
        CreateCommands();
        CreateSyncronizationPrimitives();

        //mesh.LoadMesh("Models/monkey_flat.obj");
        mesh.LoadMesh("Models/Lion.obj");
        //mesh.LoadMesh("Models/crate.obj");
    }

    void Renderer::DestroyInternal() {

        vkDeviceWaitIdle(m_RenderContext->GetDevice());

        mesh.DestroyMesh();

        DestroySyncronizationPrimitives();
        DestroyCommands();
        DestroyGraphicsPipeline();
        DestroyPipelineLayout();
        DestroyDescriptorPool();
        DestroyTextureSampler2D();
        DestroyTexture();
        DestoryDescriptorSetLayout();
        DestroyFramebuffers();
        DestroyRenderPass();
        
        RenderContext::Destroy();
    }

    void Renderer::RecreateSwapchain() {

        VK_CHECK(vkDeviceWaitIdle(m_RenderContext->GetDevice()));

        DestroyFramebuffers();
        m_RenderContext->UpdateSwapchain();
        CreateFramebuffers();
    }

    void Renderer::LoadShaderResources() {

        m_Shaders.resize(m_ShaderResources.size());

        for (U64 i = 0; i < m_ShaderResources.size(); i++) {

            const auto& resource = m_ShaderResources[i];

            m_Shaders[i].Create(m_RenderContext->GetDevice(), resource.first, resource.second);
        }
    }

    void Renderer::DestroyShaderRescources() {

        for (U64 i = 0; i < m_Shaders.size(); i++) {

            m_Shaders[i].Destroy();
        }
    }

    void Renderer::CreateRenderPass() {

        VK::AttachmentDescription depth = {};
        depth.Format = VK::FindDepthImageFormat(m_RenderContext->GetPhysicalDevice());
        depth.Samples = VK_SAMPLE_COUNT_1_BIT;
        depth.LoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth.StoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth.StencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth.StencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth.FinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VK::AttachmentDescription color = {};
        color.Format = m_RenderContext->GetSwapchainImageFormat();
        color.Samples = VK_SAMPLE_COUNT_1_BIT;
        color.LoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color.StoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        color.StencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color.StencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color.FinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VK::SubpassDescription subpass = {};
        subpass.ColorAttachments = { VK::GetAttachmentReference({ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }) };
        subpass.DepthStencilAttachment = VK::GetAttachmentReference({ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL });
        
        VK::SubpassDependency dependency = {};
        dependency.SrcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.DstSubpass = 0;
        dependency.SrcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.SrcAccessMask = 0;
        dependency.DstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.DstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VK::RenderPassCreateInfo info = {};
        info.Attachments = { VK::GetAttachmentDescription(color), VK::GetAttachmentDescription(depth) };
        info.Dependencies = { VK::GetSubpassDependency(dependency) };
        info.Subpasses = { VK::GetSubpassDescription(subpass) };
        
        m_RenderPass = VK::CreateRenderPass(m_RenderContext->GetDevice(), info);
    }

    void Renderer::DestroyRenderPass() {

        VK::DestroyRenderPass(m_RenderContext->GetDevice(), m_RenderPass);
    }

    void Renderer::CreateFramebuffers() {

        m_Framebuffers.resize(m_RenderContext->GetImageCount());

        VkExtent2D extent = m_RenderContext->GetSwapchainExtent2D();

        const auto& views = m_RenderContext->GetImageViews();
        
        for (U64 i = 0; i < m_Framebuffers.size(); i++) {

            VK::FramebufferCreateInfo info = {};
            info.Attachments = { views[i].ImageView, m_RenderContext->GetDepthView().ImageView };
            info.Layers = 1;
            info.RenderPass = m_RenderPass;
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

    void Renderer::CreatePipelineLayout() {

        VkPushConstantRange push;
        push.offset = 0;
        push.size = sizeof(glm::mat4);
        push.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VK::PipelineLayoutCreateInfo info = {};
        info.SetLayouts.push_back(m_DescriptorSetLayout);
        info.PushConstantRanges.push_back(push);

        m_Layout = VK::CreatePipelineLayout(m_RenderContext->GetDevice(), info);
    }

    void Renderer::DestroyPipelineLayout() {

        VK::DestroyPipelineLayout(m_RenderContext->GetDevice(), m_Layout);
    }

    void Renderer::CreateGraphicsPipeline() {

        LoadShaderResources();

        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkVertexInputAttributeDescription attributeDescription[3] = { {}, {} };

        attributeDescription[0].binding = 0;
        attributeDescription[0].location = 0;
        attributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescription[0].offset = offsetof(Vertex, x);

        attributeDescription[1].binding = 0;
        attributeDescription[1].location = 1;
        attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescription[1].offset = offsetof(Vertex, nx);

        attributeDescription[2].binding = 0;
        attributeDescription[2].location = 2;
        attributeDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescription[2].offset = offsetof(Vertex, u);

        VkPipelineVertexInputStateCreateInfo vertexInfo = {};
        vertexInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInfo.vertexBindingDescriptionCount = 1;
        vertexInfo.vertexAttributeDescriptionCount = 3;
        vertexInfo.pVertexAttributeDescriptions = attributeDescription;
        vertexInfo.pVertexBindingDescriptions = &bindingDescription;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        const auto& stages = VKShader::GetVulkanShaderStageInfo();

        std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

        VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.pDynamicStates = dynamicStates.data();
        dynamicStateInfo.dynamicStateCount = (U32)dynamicStates.size();

        VkPipelineDepthStencilStateCreateInfo depthStencil = {};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        VK::GraphicsPipelineCreateInfo info = {};
        info.Stages = stages;
        info.VertexInputState = vertexInfo;
        info.InputAssemblyState = inputAssembly;
        info.ViewportState = viewportState;
        info.RasterizationState = rasterizer;
        info.MultisampleState = multisampling;
        info.DepthStencilState = depthStencil;
        info.ColorBlendState = colorBlending;
        info.DynamicState = dynamicStateInfo;
        info.Layout = m_Layout;
        info.RenderPass = m_RenderPass;
    
        m_GraphicsPipeline = VK::CreateGraphicsPipeline(m_RenderContext->GetDevice(), info);
        DestroyShaderRescources();
    }

    void Renderer::DestroyGraphicsPipeline() {

        VK::DestroyGraphicsPipeline(m_RenderContext->GetDevice(), m_GraphicsPipeline);
    }

    void Renderer::CreateCommands() {

        m_CommandPools.resize(FRAME_LAG);
        m_CommandPools.reserve(FRAME_LAG);

        for (U64 i = 0; i < FRAME_LAG; i++) {

            VK::CommandPoolCreateInfo info = {};
            info.Flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            info.QueueFamilyIndex = m_RenderContext->GetGraphicsAndPresentationQueueIndex();

            m_CommandPools[i] = VK::CreateCommandPool(m_RenderContext->GetDevice(), info);

            VK::CommandBufferAllocateInfo allocateInfo = {};
            allocateInfo.CommandPool = m_CommandPools[i];
            allocateInfo.Level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocateInfo.CommandBufferCount = 1;

            auto buffer = VK::AllocateCommandBuffers(m_RenderContext->GetDevice(), allocateInfo);
            m_CommandBuffers.push_back(buffer[0]);
        }
    }

    void Renderer::DestroyCommands() {

        for (U64 i = 0; i < FRAME_LAG; i++) {

            VK::FreeCommandBuffer(m_RenderContext->GetDevice(), m_CommandPools[i], m_CommandBuffers[i]);
            VK::DestroyCommandPool(m_RenderContext->GetDevice(), m_CommandPools[i]);
        }

        m_CommandBuffers.clear();
        m_CommandPools.clear();
    }

    void Renderer::CreateSyncronizationPrimitives() {

        m_ImageAvailableSemaphores.resize(FRAME_LAG);
        m_RenderFinishedSemaphores.resize(FRAME_LAG);
        m_CommandBufferExecutedFences.resize(FRAME_LAG);

        for (U64 i = 0; i < FRAME_LAG; i++) {

            m_ImageAvailableSemaphores[i] = VK::CreateVKSemaphore(m_RenderContext->GetDevice());
            m_RenderFinishedSemaphores[i] = VK::CreateVKSemaphore(m_RenderContext->GetDevice());
            m_CommandBufferExecutedFences[i] = VK::CreateFence(m_RenderContext->GetDevice());
        }
    }

    void Renderer::DestroySyncronizationPrimitives() {

        for (U64 i = 0; i < FRAME_LAG; i++) {

            VK::DestroyVKSemaphore(m_RenderContext->GetDevice(), m_ImageAvailableSemaphores[i]);
            VK::DestroyVKSemaphore(m_RenderContext->GetDevice(), m_RenderFinishedSemaphores[i]);
            VK::DestroyFence(m_RenderContext->GetDevice(), m_CommandBufferExecutedFences[i]);
        }

        m_ImageAvailableSemaphores.clear();
        m_RenderFinishedSemaphores.clear();
        m_CommandBufferExecutedFences.clear();
    }

    void Renderer::CreateDescriptorSetLayout() {

        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = 0;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VK::DescriptorSetLayoutCreateInfo info = {};
        info.BindingCount = 1;
        info.Bindings = &binding;

        m_DescriptorSetLayout = VK::CreateDescriptorSetLayout(m_RenderContext->GetDevice(), info);
    }

    void Renderer::DestoryDescriptorSetLayout() {

        VK::DestoryDescriptorSetLayout(m_RenderContext->GetDevice(), m_DescriptorSetLayout);
    }

    void Renderer::CreateDescriptorPool() {

        VkDescriptorPoolSize size = {};
        size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        size.descriptorCount = m_RenderContext->GetImageCount();

        VK::DescriptorPoolCreateInfo info = {};
        info.MaxSets = size.descriptorCount;
        info.PoolSizeCount = 1;
        info.PoolSizes = &size;

        m_DescriptorPool.push_back(VK::CreateDescriptorPool(m_RenderContext->GetDevice(), info));
    }

    void Renderer::DestroyDescriptorPool() {

        for (auto& pool : m_DescriptorPool) {

            VK::DestoryDescriptorPool(m_RenderContext->GetDevice(), pool);
        }

        m_DescriptorPool.clear();
    }

    void Renderer::CreateDescriptorSets() {

        std::vector<VkDescriptorSetLayout> layouts(m_RenderContext->GetImageCount(), m_DescriptorSetLayout);

        VK::DescriptorSetAllocateInfo info = {};
        info.DescriptorPool = m_DescriptorPool[0];
        info.DescriptorSetCount = m_RenderContext->GetImageCount();
        info.SetLayouts = layouts.data();

        m_DescriptorSet = std::move(VK::AllocateDescriptorSets(m_RenderContext->GetDevice(), info));

        for (size_t i = 0; i < m_DescriptorSet.size(); i++) {

            VkDescriptorImageInfo imageInfo = {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = m_Texture2D->GetImageView().ImageView;
            imageInfo.sampler = m_TextureSampler2D;

            VkWriteDescriptorSet descriptorWrites = {};

            descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites.dstSet = m_DescriptorSet[i];
            descriptorWrites.dstBinding = 0;
            descriptorWrites.dstArrayElement = 0;
            descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites.descriptorCount = 1;
            descriptorWrites.pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(m_RenderContext->GetDevice(), 1, &descriptorWrites, 0, nullptr);
        }
    }

    void Renderer::CreateTextureSampler2D() {

        VkPhysicalDeviceProperties properties = VK::GetPhysicalDeviceProperties(m_RenderContext->GetPhysicalDevice());

        VK::SamplerCreateInfo info = {};
        info.MagFilter = VK_FILTER_LINEAR;
        info.MinFilter = VK_FILTER_LINEAR;
        info.AddressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.AddressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.AddressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.AnisotropyEnable = VK_TRUE;
        info.MaxAnisotropy = properties.limits.maxSamplerAnisotropy;
        info.BorderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        info.UnnormalizedCoordinates = VK_FALSE;
        info.CompareEnable = VK_FALSE;
        info.CompareOp = VK_COMPARE_OP_ALWAYS;
        info.MipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        m_TextureSampler2D = VK::CreateSampler(m_RenderContext->GetDevice(), info);
    }

    void Renderer::DestroyTextureSampler2D() {

        VK::DestroySampler(m_RenderContext->GetDevice(), m_TextureSampler2D);
    }

    void Renderer::CreateTexture() {

        m_Texture2D = new Texture2D("Models/Lion.jpg");
    }

    void Renderer::DestroyTexture() {

        delete m_Texture2D;
    }
}