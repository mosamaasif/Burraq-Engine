#pragma once

#include <vulkan/vulkan.h>
#include "VKCommon.h"

namespace BRQ {

    class VKDevice;

    class VKShader {

    public:
        enum class ShaderType {

            Invalid = 0,
            Vertex,
            Fragment,
            Geometry,
            Compute,
            TessellationControl,
            TessellationEvaluation,
        };

    private:
        VkShaderModule                  m_ShaderModule;
        VkPipelineShaderStageCreateInfo m_PipelineShaderStageInfo;

    public:
        VKShader();
        ~VKShader() = default;

        void Create(const VkDevice& device, const std::string_view filename, ShaderType type);
        void Destroy(const VkDevice& device);

        VkPipelineShaderStageCreateInfo GetPipelineShaderStageInfo() const { return m_PipelineShaderStageInfo; }

    private:
        VkShaderStageFlagBits ShaderTypeToVkShaderStageFlagBit(ShaderType type) const;
    };
}