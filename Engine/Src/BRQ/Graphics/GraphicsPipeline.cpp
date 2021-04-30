#include <BRQ.h>

#include <SPIR-V-Cross/spirv_reflect.hpp>

#include "GraphicsPipeline.h"
#include "Platform/Vulkan/RenderContext.h"

namespace BRQ {

    GraphicsPipeline::GraphicsPipeline() {

        m_Layout = VK_NULL_HANDLE;
        m_Pipeline = VK_NULL_HANDLE;
    }

    void GraphicsPipeline::Init(const GraphicsPipelineCreateInfo& info) {

        m_Stages = info.Shaders;

        std::vector<VkPipelineShaderStageCreateInfo> pipelineStages(m_Stages.size());
        std::vector<VkPushConstantRange> ranges;

        RenderContext* context = RenderContext::GetInstance();

        for (U64 i = 0; i < m_Stages.size(); i++) {

            const auto result = m_Stages[i].ShaderFilename.find(".spv");

            if (result == std::string_view::npos) {

                BRQ_CORE_ERROR("Invalid shader file extension(Only SPIRV shaders are valid)! File: {}", m_Stages[i].ShaderFilename.data());
                return;
            }

            std::vector<BYTE> code = std::move(ShaderSource(m_Stages[i].ShaderFilename));

            if (code.empty()) {

                BRQ_CORE_ERROR("Empty Shader Code (Reason: Can't Read File) File: {}", m_Stages[i].ShaderFilename.data());
                return;
            }

            ReflectionData reflection = std::move(GetSPIRVReflection(code));
       
            VkPipelineShaderStageCreateInfo stageInfo = {};

            stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo.stage = reflection.Stage;
            stageInfo.module = CreateShader(code);
            stageInfo.pName = "main";
            
            pipelineStages[i] = stageInfo;
            
            // WHY LOL
            for (U64 j = 0; j < reflection.PushConstantRanges.size(); j++) {

                ranges.push_back(reflection.PushConstantRanges[j]);
            }

            for (U64 j = 0; j < reflection.DescriptorSetLayoutData.size(); j++) {

                CreateDescriptorSetLayout(reflection.DescriptorSetLayoutData[j].CreateInfo);
            }

            m_ReflectionData.push_back(std::move(reflection));
        }

        CreatePipelineLayout(ranges);
        CreatePipeline(info, pipelineStages);

        for (U64 i = 0; i < pipelineStages.size(); i++) {

            DestroyShader(pipelineStages[i].module);
        }

        CreateDescriptorPool();
    }

    void GraphicsPipeline::Destroy() {

        const VulkanDevice& device = RenderContext::GetInstance()->GetVulkanDevice();

        VK::DestoryDescriptorPool(device.GetDevice(), m_DescriptorPool);

        for (U64 i = 0; i < m_DescriptorSetLayouts.size(); i++) {

            VK::DestoryDescriptorSetLayout(device.GetDevice(), m_DescriptorSetLayouts[i]);
        }

        VK::DestroyPipelineLayout(device.GetDevice(), m_Layout);
        VK::DestroyGraphicsPipeline(device.GetDevice(), m_Pipeline);
    }

    void GraphicsPipeline::Bind(const VkCommandBuffer& commandBuffer) const {

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
    }

    void GraphicsPipeline::BindDescriptorSets(const VkCommandBuffer& commandBuffer, const VkDescriptorSet* sets, U32 size) const {

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Layout, 0, size, sets, 0, nullptr);
    }

    void GraphicsPipeline::PushConstantData(const VkCommandBuffer& commandBuffer, PipelineStage stage, const void* data, U32 size, U32 offset) const {

        vkCmdPushConstants(commandBuffer, m_Layout, (VkShaderStageFlags)stage, offset, size, data);
    }

    std::vector<VkDescriptorSet> GraphicsPipeline::AllocateDescriptorSets() const {

        RenderContext* context = RenderContext::GetInstance();
        const VulkanDevice& device = context->GetVulkanDevice();

        VK::DescriptorSetAllocateInfo info = {};
        info.DescriptorPool = m_DescriptorPool;
        info.DescriptorSetCount = (U32)m_DescriptorSetLayouts.size();
        info.SetLayouts = m_DescriptorSetLayouts.data();

        return std::move(VK::AllocateDescriptorSets(device.GetDevice(), info));
    }

    std::vector<BYTE> GraphicsPipeline::ShaderSource(const std::string_view& filename) {

        using namespace Utilities;

        FileSystem* fs = FileSystem::GetInstance();

        std::vector<BYTE> source;

        const auto result = filename.find(".spv");

        if (result == std::string_view::npos) {

            BRQ_CORE_ERROR("Invalid shader file extension(Only SPIRV shaders are valid)! File: {}", filename.data());
            return source;
        }

        source = std::move(fs->ReadFile(filename, FileSystem::InputMode::ReadBinary));

        if (source.empty()) {

            BRQ_CORE_ERROR("Empty Shader Code (Reason: Can't Read File) File: {}", filename.data());
            return source;
        }

        return source;
    }

    ReflectionData GraphicsPipeline::GetSPIRVReflection(const std::vector<BYTE>& code) {

        ReflectionData out = {};
       

        return out;
    }

    void GraphicsPipeline::CreateDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo& createInfo) {

        const VulkanDevice& device = RenderContext::GetInstance()->GetVulkanDevice();

        VK::DescriptorSetLayoutCreateInfo info = {};
        info.BindingCount = createInfo.bindingCount;
        info.Bindings = createInfo.pBindings;

        m_DescriptorSetLayouts.push_back(VK::CreateDescriptorSetLayout(device.GetDevice(), info));
    }

    void GraphicsPipeline::CreatePipelineLayout(const std::vector<VkPushConstantRange>& ranges) {

        const VulkanDevice& device = RenderContext::GetInstance()->GetVulkanDevice();

        VK::PipelineLayoutCreateInfo info = {};
        info.SetLayouts = m_DescriptorSetLayouts;
        info.PushConstantRanges = ranges;

        m_Layout = VK::CreatePipelineLayout(device.GetDevice(), info);
    }

    VkShaderModule GraphicsPipeline::CreateShader(const std::vector<BYTE>& code) {

        const VulkanDevice& device = RenderContext::GetInstance()->GetVulkanDevice();

        VkShaderModule shader = VK_NULL_HANDLE;

        VkShaderModuleCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = code.size();
        info.pCode = (U32*)code.data();
        VK_CHECK(vkCreateShaderModule(device.GetDevice(), &info, nullptr, &shader));

        return shader;
    }

    void GraphicsPipeline::DestroyShader(VkShaderModule& shader) {

        const VulkanDevice& device = RenderContext::GetInstance()->GetVulkanDevice();

        if (shader != VK_NULL_HANDLE) {

            vkDestroyShaderModule(device.GetDevice(), shader, nullptr);
            shader = VK_NULL_HANDLE;
        }
    }

    void GraphicsPipeline::CreatePipeline(const GraphicsPipelineCreateInfo& info, std::vector<VkPipelineShaderStageCreateInfo>& stageInfos) {

        RenderContext* context = RenderContext::GetInstance();
        const VulkanDevice& device = context->GetVulkanDevice();

        const std::vector<BufferElement>& elements = info.Layout.GetElements();

        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = (U32)info.Layout.GetStride();
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::vector<VkVertexInputAttributeDescription> attributeDescription(elements.size());

        for (U64 i = 0; i < elements.size(); i++) {

            attributeDescription[i].binding = 0;
            attributeDescription[i].location = (U32)i;
            attributeDescription[i].format = ToVulkanFormat(elements[i].Type);
            attributeDescription[i].offset = elements[i].Offset;
        }

        VkPipelineVertexInputStateCreateInfo vertexInfo = {};
        vertexInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInfo.vertexBindingDescriptionCount = 1;
        vertexInfo.vertexAttributeDescriptionCount = (U32)attributeDescription.size();
        vertexInfo.pVertexAttributeDescriptions = attributeDescription.data();
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
        rasterizer.lineWidth = 1.0f;
        rasterizer.depthBiasEnable = VK_FALSE;

        if (info.Flags & PolygonModeLine) {

            rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
        }
        else {

            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        }

        if (info.Flags & EnableCulling) {

            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

            if (info.Flags & CullModeFrontFace) {

                rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
            }
            else {
                rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            }
        }

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

        std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

        VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.pDynamicStates = dynamicStates.data();
        dynamicStateInfo.dynamicStateCount = (U32)dynamicStates.size();

        VkPipelineDepthStencilStateCreateInfo depthStencil = {};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

        if (info.Flags & DepthWriteEnabled) {

            depthStencil.depthWriteEnable = VK_TRUE;
        }

        if (info.Flags & DepthTestEnabled) {

            depthStencil.depthTestEnable = VK_TRUE;
        }

        if (info.Flags & DepthCompareLess) {

            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        }
        else if(info.Flags & DepthCompareEqual) {

            depthStencil.depthCompareOp = VK_COMPARE_OP_EQUAL;
        }
        else if (info.Flags & DepthCompareGreater) {

            depthStencil.depthCompareOp = VK_COMPARE_OP_GREATER;
        }
        else if ((info.Flags & DepthCompareLess) && (info.Flags & DepthCompareEqual)) {

            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        }
        else if ((info.Flags & DepthCompareGreater) && (info.Flags & DepthCompareEqual)) {

            depthStencil.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
        }

        VK::GraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.Stages = stageInfos;
        pipelineInfo.VertexInputState = vertexInfo;
        pipelineInfo.InputAssemblyState = inputAssembly;
        pipelineInfo.ViewportState = viewportState;
        pipelineInfo.RasterizationState = rasterizer;
        pipelineInfo.MultisampleState = multisampling;
        pipelineInfo.DepthStencilState = depthStencil;
        pipelineInfo.ColorBlendState = colorBlending;
        pipelineInfo.DynamicState = dynamicStateInfo;
        pipelineInfo.Layout = m_Layout;
        pipelineInfo.RenderPass = context->GetRenderPass();

        m_Pipeline = VK::CreateGraphicsPipeline(device.GetDevice(), pipelineInfo);
    }

    void GraphicsPipeline::CreateDescriptorPool() {

        RenderContext* context = RenderContext::GetInstance();
        const VulkanDevice& device = context->GetVulkanDevice();

        VkDescriptorPoolSize sizes[] = {

            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 16 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 16 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 16 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 16 },
        };

        VK::DescriptorPoolCreateInfo info = {};
        info.MaxSets = 256;
        info.PoolSizeCount = sizeof(sizes) / sizeof(sizes[0]);
        info.PoolSizes = sizes;

        m_DescriptorPool = VK::CreateDescriptorPool(device.GetDevice(), info);
    }
}