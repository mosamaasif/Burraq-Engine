#pragma once

#include <BRQ.h>
#include <vulkan/vulkan.h>

namespace BRQ {

    class VKDevice;
    class VKSwapchain;
    class VKRenderPass;
    class VKPipelineLayout;
    class VKCommandBuffer;

    class VKGraphicsPipeline {

    private:
        VkPipeline			m_GraphicsPipeline;
        const VKDevice*		m_Device;

    public:
        VKGraphicsPipeline();
        ~VKGraphicsPipeline() = default;

        void Bind(const VKCommandBuffer* commandBuffer);

        const VkPipeline& GetGraphicsPipeline() const { return m_GraphicsPipeline; }

        void Create(const VKDevice* device,
                    const VKSwapchain* swapchain,
                    const VKPipelineLayout* layout,
                    const VKRenderPass* renderpass,
                    const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages);

        void Destroy();
    };
}