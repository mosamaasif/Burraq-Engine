#pragma once

#include <BRQ.h>

#include <vulkan/vulkan.h>
#include "VKInitializers.h"

namespace BRQ { namespace VK {

    static void CommandBeginRenderPass(const RenderPassBeginInfo& info = {}) {

        VkClearValue values[] = { info.ColorValues, info.DepthValues };

        VkRenderPassBeginInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        createInfo.renderPass = info.RenderPass;
        createInfo.framebuffer = info.Framebuffer;
        createInfo.renderArea = info.RenderArea;
        createInfo.clearValueCount = 2 ;
        createInfo.pClearValues = values;
        
        vkCmdBeginRenderPass(info.CommandBuffer, &createInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    static void CommandEndRenderPass(const VkCommandBuffer& commandBuffer) {

        vkCmdEndRenderPass(commandBuffer);
    }

} }