#include <BRQ.h>

#include "Renderer.h"

#include "Application/Window.h"
#include "Utilities/VulkanMemoryAllocator.h"

#include "Graphics/Mesh.h"

#include "Platform/Vulkan/RenderContext.h"
#include "Platform/Vulkan/VKCommands.h"

namespace BRQ {

	// this shouldnt be here
	Mesh mesh;

	Renderer* Renderer::s_Renderer = nullptr;
	std::vector<std::pair<std::string, VKShader::ShaderType>> Renderer::s_ShaderResources;

	Renderer::Renderer()
		: m_RenderContext(nullptr), m_RenderPass(nullptr),
		m_Layout(nullptr), m_GraphicsPipeline(nullptr), m_Window(nullptr) { }

	void Renderer::Init(const Window* window) {

		s_Renderer = new Renderer();

		s_Renderer->InitInternal(window);
	}

	void Renderer::Shutdown() {

		s_Renderer->DestroyInternal();

		delete s_Renderer;
	}

	void Renderer::Present() {

		U32 index = m_RenderContext->GetCurrentIndex();

		VK::WaitForFence(m_RenderContext->GetDevice(), m_CommandBufferExecutedFences[index]);
		VK::ResetFence(m_RenderContext->GetDevice(), m_CommandBufferExecutedFences[index]);

		VK::ResetCommandPool(m_RenderContext->GetDevice(), m_CommandPools[index]);

		VkCommandBuffer buffer = m_CommandBuffers[index];

		U32 imageIndex = m_RenderContext->AcquireImageIndex(m_ImageAvailableSemaphores[index]);

		if (m_RenderContext->GetSwapchainStatus() == VK::SwapchainStatus::NotReady) {

			RecreateSwapchain();
		}

		VK::CommandBufferBeginInfo beginInfo = {};
		beginInfo.CommandBuffer = buffer;
		beginInfo.Flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VK::CommandBufferBegin(beginInfo);

		VK::RenderPassBeginInfo info = {};
		info.CommandBuffer = buffer;
		info.RenderPass = m_RenderPass;
		info.Framebuffer = m_Framebuffers[imageIndex];
		info.RenderArea.extent = m_RenderContext->GetSwapchainExtent2D();
		info.RenderArea.offset = { 0, 0 };

		VK::CommandBeginRenderPass(info);

		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

		VkExtent2D extent = m_RenderContext->GetSwapchainExtent2D();

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (F32)extent.width;
		viewport.height = (F32)extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;

		vkCmdSetViewport(buffer, 0, 1, &viewport);
		vkCmdSetScissor(buffer, 0, 1, &scissor);

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(buffer, 0, 1, &mesh.GetVertexBuffer(), &offset);
		vkCmdBindIndexBuffer(buffer, mesh.GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(buffer, (U32)mesh.GetIndexCount(), 1, 0, 0, 0);

		VK::CommandEndRenderPass(buffer);

		VK::CommandBufferEnd(buffer);

		VK::QueueSubmitInfo submitInfo = {};
		submitInfo.WaitSemaphoreCount = 1;
		submitInfo.WaitSemaphores = &m_ImageAvailableSemaphores[index];
		submitInfo.WaitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		submitInfo.CommandBufferCount = 1;
		submitInfo.CommandBuffers = &buffer;
		submitInfo.SignalSemaphoreCount = 1;
		submitInfo.SignalSemaphores = &m_RenderFinishedSemaphores[index];
		submitInfo.QueueFamilyIndex = m_RenderContext->GetGraphicsAndPresentationQueueIndex();
		submitInfo.Queue = m_RenderContext->GetGraphicsAndPresentationQueue();
		submitInfo.CommandBufferExecutedFence = m_CommandBufferExecutedFences[index];

		VK::QueueSubmit(submitInfo);

		m_RenderContext->Present(m_RenderFinishedSemaphores[index]);

		if (m_RenderContext->GetSwapchainStatus() == VK::SwapchainStatus::NotReady) {

			RecreateSwapchain();
		}
	}

	void Renderer::InitInternal(const Window* window) {

		m_Window = window;

		RenderContext::Init(window, { { VK::QueueType::Graphics, 1.0f } });

		m_RenderContext = RenderContext::GetInstance();

		CreateRenderPass();
		CreateFramebuffers();
		CreatePipelineLayout();
		CreateGraphicsPipeline();
		CreateCommands();
		CreateSyncronizationPrimitives();

		mesh.LoadMesh("Src/Models/monkey_flat.obj");
	}

	void Renderer::DestroyInternal() {

		vkDeviceWaitIdle(m_RenderContext->GetDevice());

		mesh.DestroyMesh();

		DestroySyncronizationPrimitives();
		DestroyCommands();
		DestroyGraphicsPipeline();
		DestroyPipelineLayout();
		DestroyFramebuffers();
		DestroyRenderPass();
		
		RenderContext::Destroy();
	}

	void Renderer::RecreateSwapchain() {

		VK_CHECK(vkDeviceWaitIdle(m_RenderContext->GetDevice()));

		DestroyFramebuffers();
		m_RenderContext->UpdateSwapchain();
		m_RenderContext->UpdateDepthResources();
		CreateFramebuffers();
	}

	void Renderer::LoadShaderResources() {

		m_Shaders.resize(s_ShaderResources.size());

		for (U64 i = 0; i < s_ShaderResources.size(); i++) {

			const auto& resource = s_ShaderResources[i];

			m_Shaders[i].Create(m_RenderContext->GetDevice(), resource.first, resource.second);
		}
	}

	void Renderer::DestroyShaderRescources() {

		for (U64 i = 0; i < m_Shaders.size(); i++) {

			m_Shaders[i].Destroy();
		}
	}

	void Renderer::CreateRenderPass() {

		VK::AttachmentDescription depth = {};
		depth.Format = VK::FindDepthImageFormat(m_RenderContext->GetPhysicalDevice());
		depth.Samples = VK_SAMPLE_COUNT_1_BIT;
		depth.LoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth.StoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth.StencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth.StencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth.FinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VK::AttachmentDescription color = {};
		color.Format = m_RenderContext->GetSwapchainImageFormat();
		color.Samples = VK_SAMPLE_COUNT_1_BIT;
		color.LoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color.StoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		color.StencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color.StencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color.FinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VK::SubpassDescription subpass = {};
		subpass.ColorAttachments = { VK::GetAttachmentReference({ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }) };
		subpass.DepthStencilAttachment = VK::GetAttachmentReference({ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL });
		
		VK::SubpassDependency dependency = {};
		dependency.SrcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.DstSubpass = 0;
		dependency.SrcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.SrcAccessMask = 0;
		dependency.DstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.DstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		VK::RenderPassCreateInfo info = {};
		info.Attachments = { VK::GetAttachmentDescription(color), VK::GetAttachmentDescription(depth) };
		info.Dependencies = { VK::GetSubpassDependency(dependency) };
		info.Subpasses = { VK::GetSubpassDescription(subpass) };
		
		m_RenderPass = VK::CreateRenderPass(m_RenderContext->GetDevice(), info);
	}

	void Renderer::DestroyRenderPass() {

		VK::DestroyRenderPass(m_RenderContext->GetDevice(), m_RenderPass);
	}

	void Renderer::CreateFramebuffers() {

		m_Framebuffers.resize(m_RenderContext->GetImageCount());

		VkExtent2D extent = m_RenderContext->GetSwapchainExtent2D();

		const auto& views = m_RenderContext->GetImageViews();
		
		for (U64 i = 0; i < m_Framebuffers.size(); i++) {

			VK::FramebufferCreateInfo info = {};
			info.Attachments = { views[i].ImageView, m_RenderContext->GetDepthView().ImageView };
			info.Layers = 1;
			info.RenderPass = m_RenderPass;
			info.Height = extent.height;
			info.Width = extent.width;

			m_Framebuffers[i] = VK::CreateFramebuffer(m_RenderContext->GetDevice(), info);
		}
	}

	void Renderer::DestroyFramebuffers() {

		for (U64 i = 0; i < m_Framebuffers.size(); i++) {

			VK::DestroyFramebuffer(m_RenderContext->GetDevice(), m_Framebuffers[i]);
		}

		m_Framebuffers.clear();
	}

	void Renderer::CreatePipelineLayout() {

		VK::PipelineLayoutCreateInfo info = {};
		m_Layout = VK::CreatePipelineLayout(m_RenderContext->GetDevice(), info);
	}

	void Renderer::DestroyPipelineLayout() {

		VK::DestroyPipelineLayout(m_RenderContext->GetDevice(), m_Layout);
	}

	void Renderer::CreateGraphicsPipeline() {

		LoadShaderResources();

		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkVertexInputAttributeDescription attributeDescription[2] = { {}, {} };

		attributeDescription[0].binding = 0;
		attributeDescription[0].location = 0;
		attributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescription[0].offset = offsetof(Vertex, x);

		attributeDescription[1].binding = 0;
		attributeDescription[1].location = 1;
		attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescription[1].offset = offsetof(Vertex, ny);

		VkPipelineVertexInputStateCreateInfo vertexInfo = {};
		vertexInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInfo.vertexBindingDescriptionCount = 1;
		vertexInfo.vertexAttributeDescriptionCount = 2;
		vertexInfo.pVertexAttributeDescriptions = attributeDescription;
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
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

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

		const auto& stages = VKShader::GetVulkanShaderStageInfo();

		std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.pDynamicStates = dynamicStates.data();
		dynamicStateInfo.dynamicStateCount = (U32)dynamicStates.size();

		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_GREATER;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		VK::GraphicsPipelineCreateInfo info = {};
		info.Stages = stages;
		info.VertexInputState = vertexInfo;
		info.InputAssemblyState = inputAssembly;
		info.ViewportState = viewportState;
		info.RasterizationState = rasterizer;
		info.MultisampleState = multisampling;
		info.DepthStencilState = depthStencil;
		info.ColorBlendState = colorBlending;
		info.DynamicState = dynamicStateInfo;
		info.Layout = m_Layout;
		info.RenderPass = m_RenderPass;
	
		m_GraphicsPipeline = VK::CreateGraphicsPipeline(m_RenderContext->GetDevice(), info);
		DestroyShaderRescources();
	}

	void Renderer::DestroyGraphicsPipeline() {

		VK::DestroyGraphicsPipeline(m_RenderContext->GetDevice(), m_GraphicsPipeline);
	}

	void Renderer::CreateCommands() {

		m_CommandPools.resize(FRAME_LAG);
		m_CommandPools.reserve(FRAME_LAG);

		for (U64 i = 0; i < FRAME_LAG; i++) {

			VK::CommandPoolCreateInfo info = {};
			info.Flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			info.QueueFamilyIndex = m_RenderContext->GetGraphicsAndPresentationQueueIndex();

			m_CommandPools[i] = VK::CreateCommandPool(m_RenderContext->GetDevice(), info);

			VK::CommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.CommandPool = m_CommandPools[i];
			allocateInfo.Level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.CommandBufferCount = 1;

			auto buffer = VK::AllocateCommandBuffers(m_RenderContext->GetDevice(), allocateInfo);
			m_CommandBuffers.push_back(buffer[0]);
		}
	}

	void Renderer::DestroyCommands() {

		for (U64 i = 0; i < FRAME_LAG; i++) {

			VK::FreeCommandBuffer(m_RenderContext->GetDevice(), m_CommandPools[i], m_CommandBuffers[i]);
			VK::DestroyCommandPool(m_RenderContext->GetDevice(), m_CommandPools[i]);
		}

		m_CommandBuffers.clear();
		m_CommandPools.clear();
	}

	void Renderer::CreateSyncronizationPrimitives() {

		m_ImageAvailableSemaphores.resize(FRAME_LAG);
		m_RenderFinishedSemaphores.resize(FRAME_LAG);
		m_CommandBufferExecutedFences.resize(FRAME_LAG);

		for (U64 i = 0; i < FRAME_LAG; i++) {

			m_ImageAvailableSemaphores[i] = VK::CreateVKSemaphore(m_RenderContext->GetDevice());
			m_RenderFinishedSemaphores[i] = VK::CreateVKSemaphore(m_RenderContext->GetDevice());
			m_CommandBufferExecutedFences[i] = VK::CreateFence(m_RenderContext->GetDevice());
		}
	}

	void Renderer::DestroySyncronizationPrimitives() {

		for (U64 i = 0; i < FRAME_LAG; i++) {

			VK::DestroyVKSemaphore(m_RenderContext->GetDevice(), m_ImageAvailableSemaphores[i]);
			VK::DestroyVKSemaphore(m_RenderContext->GetDevice(), m_RenderFinishedSemaphores[i]);
			VK::DestroyFence(m_RenderContext->GetDevice(), m_CommandBufferExecutedFences[i]);
		}

		m_ImageAvailableSemaphores.clear();
		m_RenderFinishedSemaphores.clear();
		m_CommandBufferExecutedFences.clear();
	}
}