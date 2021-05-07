#include <BRQ.h>

#include "Material.h"

#include "Platform/Vulkan/RenderContext.h"

namespace BRQ {

    Material::Material() {

        m_Texture = nullptr;
        m_Pipeline = nullptr;
        m_Loaded = false;
    }

    Material::~Material() {


        // do it later
    }

    void Material::SetPipeline(const GraphicsPipeline* pipeline) {

        m_Pipeline = pipeline;
    }

    void Material::SetTexture(const Texture* texture) {

        m_Texture = texture;
    }

    void Material::BuildMaterial() {

        RenderContext* context = RenderContext::GetInstance();
        const VulkanDevice& device = context->GetVulkanDevice();

        for (U64 i = 0; i < FRAME_LAG; i++) {

            m_DescriptorSets[i] = std::move(m_Pipeline->AllocateDescriptorSets());
        }

        const std::vector<ReflectionData>& reflection = m_Pipeline->GetReflectionData();

        for (U64 ref = 0; ref < reflection.size(); ref++) {

            const std::vector<DescriptorSetLayoutData>& layout = reflection[ref].DescriptorSetLayoutData;

            for (U64 frame = 0; frame < FRAME_LAG; frame++) {

                for (U64 layoutIndex = 0; layoutIndex < layout.size(); layoutIndex++) {
            
                    const DescriptorSetLayoutData& data = layout[layoutIndex];

                    for (U64 binding = 0; binding < data.Bindings.size(); binding++) {
                
                        const VkDescriptorSetLayoutBinding& bind = data.Bindings[binding];
                
                        switch (bind.descriptorType) {
                
                        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                        {
                            VkDescriptorImageInfo imageInfo = {};
                            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                            imageInfo.imageView = m_Texture->GetImageView();
                            imageInfo.sampler = m_Texture->GetSampler();

                            VkWriteDescriptorSet descriptorWrites = {};
                            descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                            descriptorWrites.dstSet = m_DescriptorSets[frame][layoutIndex];
                            descriptorWrites.dstBinding = (U32)binding;
                            descriptorWrites.dstArrayElement = 0;
                            descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                            descriptorWrites.descriptorCount = 1;
                            descriptorWrites.pImageInfo = &imageInfo;

                            vkUpdateDescriptorSets(device.GetDevice(), 1, &descriptorWrites, 0, nullptr);
                
                            break;
                        }
                        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                        {
                
                            break;
                        }
                        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
                        {
                
                            break;
                        }
                        default:
                        {
                            BRQ_ERROR("Shader Descriptor Type is NOT supported!");
                            BRQ_ASSERT(false)
                        }
                        }
                    }
                }
            }
        }
        
        m_Loaded = true;
    }

    void Material::SetUniforms(void* data, U32 size, U32 index) {

        const auto vma = VulkanMemoryAllocator::GetInstance();

        void* buffer = vma->MapMemory(m_UniformBuffer[index]);

        memcpy_s(buffer, size, data, size);

        vma->UnMapMemory(m_UniformBuffer[index]);
    }

    void Material::Bind(const MaterialBindInfo& info) const {

        if (m_Loaded) {

            m_Pipeline->Bind(info.CommandBuffer);

            m_Pipeline->PushConstantData(info.CommandBuffer, 
                                         info.PushConstantPipelineStage, 
                                         info.PushConstantData,
                                         info.PushConstantSize, info.PushConstantOffset);

            m_Pipeline->BindDescriptorSets(info.CommandBuffer, 
                                           m_DescriptorSets[info.FrameIndex].data(), 
                                           (U32)m_DescriptorSets[info.FrameIndex].size());
        }
    }

}