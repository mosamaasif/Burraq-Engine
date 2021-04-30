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

        // this is alot of work i think it worth it now
        std::vector<VkWriteDescriptorSet> writes;

        const std::vector<ReflectionData>& reflection = m_Pipeline->GetReflectionData();

        for (U64 ref = 0; ref < reflection.size(); ref++) {

            const std::vector<DescriptorSetLayoutData>& layout = reflection[ref].DescriptorSetLayoutData;

            for (U64 l = 0; l < layout.size(); l++) {

                const DescriptorSetLayoutData& data = layout[l];

                // too much work and i dont wanna waste my time
                // i am already outta time out here

                //for (U64 binding = 0; binding < data.Bindings.size(); binding++) {
                //
                //    const auto& bind = data.Bindings[binding];
                //
                //    switch (bind.descriptorType) {
                //
                //    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                //    {
                //
                //
                //        break;
                //    }
                //    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                //    {
                //
                //        break;
                //    }
                //    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
                //    {
                //
                //        break;
                //    }
                //    default:
                //    {
                //        BRQ_ERROR("Shader Descriptor Type is NOT supported!");
                //        BRQ_ASSERT(false)
                //    }
                //    }
                //
                //}
                //
                 BRQ_ASSERT(true);
            }

                //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
                //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
                //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,



        }

            // for now only supporting texture descriptor
            // this is bad
        for (U64 i = 0; i < FRAME_LAG; i++) {

            for (U64 set = 0; set < m_DescriptorSets[i].size(); set++) {

                // need more info from reflection
                VkDescriptorImageInfo imageInfo = {};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = m_Texture->GetImageView();
                imageInfo.sampler = m_Texture->GetSampler();

                VkWriteDescriptorSet descriptorWrites = {};
                descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites.dstSet = m_DescriptorSets[i][set];
                descriptorWrites.dstBinding = 0;
                descriptorWrites.dstArrayElement = 0;
                descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites.descriptorCount = 1;
                descriptorWrites.pImageInfo = &imageInfo;

                vkUpdateDescriptorSets(device.GetDevice(), 1, &descriptorWrites, 0, nullptr);
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