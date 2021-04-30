#pragma once

#include "GraphicsPipeline.h"
#include "Texture.h"

#include <unordered_map>

namespace BRQ {

    struct MaterialBindInfo {

        VkCommandBuffer CommandBuffer;
        PipelineStage   PushConstantPipelineStage;
        void*           PushConstantData;
        U32             PushConstantSize;
        U32             PushConstantOffset;
        U32             FrameIndex;
    };

    class Material {

    private:
        const GraphicsPipeline*              m_Pipeline;
        const Texture*                       m_Texture;
        std::vector<VkDescriptorSet>         m_DescriptorSets[FRAME_LAG];
        VK::Buffer                           m_UniformBuffer[FRAME_LAG];
        bool                                 m_Loaded;
        std::unordered_map<const char*, U32> m_UniformCache;

    public:
        Material();
        ~Material();

        void SetPipeline(const GraphicsPipeline* pipeline);
        void SetTexture(const Texture* texture);

        void BuildMaterial();

        void SetUniforms(void* data, U32 size, U32 index);

        void Bind(const MaterialBindInfo& info) const;
    };
}