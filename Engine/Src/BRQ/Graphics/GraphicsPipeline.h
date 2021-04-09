#pragma once

#include "Platform/Vulkan/VulkanHelpers.h"
#include "BufferLayout.h"

namespace BRQ {

    struct ShaderStage {

        std::string ShaderFilename;
    };

    enum GraphicsPipelineFlags {

        // LINE MODE NOT FULLY SUPPORTED RIGHT NOW
        PolygonModeLine     = BIT(0),
        EnableCulling       = BIT(1),
        CullModeFrontFace   = BIT(2),
        DepthWriteEnabled   = BIT(3),
        DepthTestEnabled    = BIT(4),
        DepthCompareLess    = BIT(5),
        DepthCompareEqual   = BIT(6),
        DepthCompareGreater = BIT(7)
    };

    enum class PipelineStage {

        Vertex   = VK_SHADER_STAGE_VERTEX_BIT,
        Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
        Compute  = VK_SHADER_STAGE_COMPUTE_BIT,
        All      = VK_SHADER_STAGE_ALL,
    };

    struct GraphicsPipelineCreateInfo {

        BufferLayout             Layout;
        GraphicsPipelineFlags    Flags;
        std::vector<ShaderStage> Shaders;
    };

    class GraphicsPipeline {

    private:
        VkPipelineLayout                   m_Layout;
        VkPipeline                         m_Pipeline;
        std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
        std::vector<ShaderStage>           m_Stages;

    public:
        GraphicsPipeline();
        ~GraphicsPipeline() = default;

        void Init(const GraphicsPipelineCreateInfo& info);
        void Destroy();

        void Bind(const VkCommandBuffer& commandBuffer);
        void BindDescriptorSets(const VkCommandBuffer& commandBuffer, const VkDescriptorSet* sets, U32 size);
        const std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts() const { return m_DescriptorSetLayouts; }
        void PushConstantData(const VkCommandBuffer& commandBuffer, PipelineStage stage, const void* data, U32 size, U32 offset);

    private:

        struct DescriptorSetLayoutData {

            U32                                       SetNumber;
            VkDescriptorSetLayoutCreateInfo           CreateInfo;
            std::vector<VkDescriptorSetLayoutBinding> Bindings;
        };


        struct ReflectionData {

            VkShaderStageFlagBits                Stage;
            std::vector<VkPushConstantRange>     PushConstantRanges;
            std::vector<DescriptorSetLayoutData> DescriptorSetLayoutData;

            ReflectionData() = default;

            ReflectionData(ReflectionData&& other) noexcept {

                Stage = other.Stage;
                PushConstantRanges = std::move(other.PushConstantRanges);
                DescriptorSetLayoutData = std::move(other.DescriptorSetLayoutData);
            }
        };

        std::vector<BYTE> ShaderSource(const std::string_view& filename);

        GraphicsPipeline::ReflectionData GetSPIRVReflection(const std::vector<BYTE>& code);

        void CreateDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo& createInfos);
        void CreatePipelineLayout(const std::vector<VkPushConstantRange>& ranges);

        VkShaderModule CreateShader(const std::vector<BYTE>& code);
        void DestroyShader(VkShaderModule& shader);

        void CreatePipeline(const GraphicsPipelineCreateInfo& info, std::vector<VkPipelineShaderStageCreateInfo>& stageInfos);
    };
}