#include <BRQ.h>

#include "VKShader.h"

namespace BRQ {

    std::vector<VkPipelineShaderStageCreateInfo> VKShader::s_ShaderStages(0);

    VKShader::VKShader()
        : m_ShaderModule(VK_NULL_HANDLE), m_Device(nullptr) { }

    void VKShader::Create(const VkDevice& device, const std::string_view filename, ShaderType type) {

        m_Device = device;

        const auto result = filename.find(".spv");

        if (result == std::string_view::npos) {

            BRQ_CORE_ERROR("Invalid shader file extension(Only SPIRV shaders are valid)! File: {}", filename.data());
            return;
        }

        std::vector<BYTE> code = Utilities::FileSystem::GetInstance()->ReadFile(filename, Utilities::FileSystem::InputMode::ReadBinary);

        if (code.empty()) {

            BRQ_CORE_ERROR("Empty Shader Code (Reason: Can't Read File) File: {}", filename.data());
            return;
        }

        {
            VkShaderModuleCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            info.codeSize = code.size();
            info.pCode = (U32*)code.data();
            VK_CHECK(vkCreateShaderModule(device, &info, nullptr, &m_ShaderModule));
        }

        {
            VkPipelineShaderStageCreateInfo info = {};

            info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.stage = ShaderTypeToVkShaderStageFlagBit(type);
            info.module = m_ShaderModule;
            info.pName = "main";
            s_ShaderStages.push_back(info);
        }
    }

    // Destroy after pipeline creation
    void VKShader::Destroy() {

        for (auto it = s_ShaderStages.begin(); it != s_ShaderStages.end(); it++) {

            const auto& stageinfo = *it;

            if (stageinfo.module == m_ShaderModule) {

                s_ShaderStages.erase(it);
                break;
            }
        }

        if (m_ShaderModule) {

            vkDestroyShaderModule(m_Device, m_ShaderModule, nullptr);
            m_ShaderModule = VK_NULL_HANDLE;
        }
    }

    VkShaderStageFlagBits VKShader::ShaderTypeToVkShaderStageFlagBit(ShaderType type) const {

        switch (type) {

        case ShaderType::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderType::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderType::Geometry:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderType::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        case ShaderType::TessellationControl:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case ShaderType::TessellationEvaluation:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        default:
            return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        }
    }
}