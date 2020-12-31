#pragma once

#include "BRQ.h"

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
        VkShaderModule                                      m_ShaderModule;
        const VKDevice*                                     m_Device;

        static std::vector<VkPipelineShaderStageCreateInfo> s_ShaderStages;

    public:
        VKShader();
        VKShader(const VKDevice* device, const std::string_view filename, ShaderType type);

        void Create(const VKDevice* device, const std::string_view filename, ShaderType type);
        void Destroy();

        const VkShaderModule& GetVulkanShaderModule() const { return m_ShaderModule; }

        static std::vector<VkPipelineShaderStageCreateInfo> GetVulkanShaderStageInfo() { return s_ShaderStages; };

    private:
        VkShaderStageFlagBits ShaderTypeToVkShaderStageFlagBit(ShaderType type) const;
    };
}