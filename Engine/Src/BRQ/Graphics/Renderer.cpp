#include <BRQ.h>

#include "Renderer.h"

#include "Application/Window.h"
#include "Utilities/VulkanMemoryAllocator.h"

#include "Platform/Vulkan/VKIndexBuffer.h"
#include "Platform/Vulkan/VKVertexBuffer.h"
#include "Graphics/Mesh.h"

namespace BRQ {

	// this shouldnt be here
	Mesh mesh;

	VKVertexBuffer vertexBuffer;
	VKIndexBuffer indexBuffer;

	Renderer* Renderer::s_Renderer = nullptr;
	std::vector<std::pair<std::string, VKShader::ShaderType>> Renderer::s_ShaderResources;

	Renderer::Renderer()
		: m_VulkanInstance(nullptr), m_Surface(nullptr), m_Device(nullptr),
		m_Swapchain(nullptr), m_RenderPass(nullptr), m_Layout(nullptr),
		m_GraphicsPipeline(nullptr), m_Window(nullptr) { }

	void Renderer::Init(const Window* window) {

		s_Renderer = new Renderer();

		s_Renderer->InitInternal(window);
	}

	void Renderer::Shutdown() {

		s_Renderer->DestroyInternal();

		delete s_Renderer;
	}

	void Renderer::Begin()
	{
	}

	void Renderer::End()
	{
	}

	void Renderer::Present() {

		U32 index = m_Swapchain->GetCurrentImageIndex();

		m_CommandBufferExecutedFences[index].Wait();
		m_CommandBufferExecutedFences[index].Reset();

		if (m_Swapchain->GetSwapchainStatus() == VKSwapchain::SwapchainStatus::NotReady) {

			RecreateSwapchain();
			return;
		}

		m_CommandPools[index].Reset();

		VKCommandBuffer* buffer = &m_CommandBuffers[index];

		m_Swapchain->AcquireNextImageIndex(&m_ImageAvailableSemaphores[index]);
		U32 imageIndex = m_Swapchain->GetAcquiredNextImageIndex();

		buffer->Begin();
		m_RenderPass->Begin(buffer, &m_Swapchain->GetSwapchainFramebuffers()[imageIndex]);

		m_GraphicsPipeline->Bind(buffer);

		VkExtent2D extent = m_Swapchain->GetSwapchainExtent2D();

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = extent.width;
		viewport.height = extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;

		vkCmdSetViewport(buffer->GetCommandBuffer(), 0, 1, &viewport);
		vkCmdSetScissor(buffer->GetCommandBuffer(), 0, 1, &scissor);

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(buffer->GetCommandBuffer(), 0, 1, &vertexBuffer.GetVertexBuffer(), &offset);
		vkCmdBindIndexBuffer(buffer->GetCommandBuffer(), indexBuffer.GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(buffer->GetCommandBuffer(), mesh.GetIndices().size(), 1, 0, 0, 0);

		m_RenderPass->End(buffer);
		buffer->End();


		buffer->Submit(m_Device, &m_ImageAvailableSemaphores[index], &m_RenderFinishedSemaphores[index], &m_CommandBufferExecutedFences[index]);

		m_Swapchain->Present(&m_RenderFinishedSemaphores[index]);

		if (m_Swapchain->GetSwapchainStatus() == VKSwapchain::SwapchainStatus::NotReady) {

			RecreateSwapchain();
			return;
		}
	}

	void Renderer::InitInternal(const Window* window) {

		m_Window = window;

		CreateInstance();
		CreateSurface();
		CreateDevice();
		CreateSwapchain();
		CreateRenderPass();
		CreateFramebuffers();
		CreatePipelineLayout();
		CreateGraphicsPipeline();
		CreateCommands();
		CreateSyncronizationPrimitives();

		VulkanMemoryAllocator::Init(m_VulkanInstance, m_Device);

		mesh.LoadMesh("Src/Models/kitten.obj");
		//mesh.LoadMesh("Src/Models/Mickeytest.obj");
		//mesh.LoadMesh("Src/Models/pirate.obj");
		vertexBuffer.Create(mesh.GetVertices());
		indexBuffer.Create(mesh.GetIndices());
	}

	void Renderer::DestroyInternal() {

		m_Device->WaitDeviceIdle();

		VulkanMemoryAllocator::GetInstance()->DestroyBuffer(vertexBuffer.GetBufferInfo());
		VulkanMemoryAllocator::GetInstance()->DestroyBuffer(indexBuffer.GetBufferInfo());
		VulkanMemoryAllocator::Shutdown();

		DestroySyncronizationPrimitives();
		DestroyCommands();
		DestroyGraphicsPipeline();
		DestroyPipelineLayout();
		DestroyFramebuffers();
		DestroyRenderPass();
		DestroySwapchain();
		DestroyDevice();
		DestroySurface();
		DestroyInstance();
	}

	void Renderer::RecreateSwapchain() {

		auto capabilities = m_Surface->GetSurfaceCapabilities(m_Device);
		auto dimensions = m_Swapchain->GetSwapchainExtent2D();
		
		if (capabilities.currentExtent.width == dimensions.width && capabilities.currentExtent.height == dimensions.height) {

			return;
		}

		m_Device->WaitDeviceIdle();

		m_Swapchain->DestroySwapchainFramebuffers();
		m_Swapchain->Create(m_Device, m_Surface, m_Window);
		m_Swapchain->CreateSwapchainFramebuffers(m_RenderPass);
	}

	void Renderer::LoadShaderResources() {

		m_Shaders.resize(s_ShaderResources.size());

		for (U64 i = 0; i < s_ShaderResources.size(); i++) {

			const auto& resource = s_ShaderResources[i];

			m_Shaders[i].Create(m_Device, resource.first, resource.second);
		}
	}

	void Renderer::DestroyShaderRescources() {

		for (U64 i = 0; i < m_Shaders.size(); i++) {

			m_Shaders[i].Destroy();
		}
	}

	void Renderer::CreateInstance() {

		m_VulkanInstance = new VKInstance();
		m_VulkanInstance->Create();
	}

	void Renderer::DestroyInstance() {

		if (m_VulkanInstance) {

			m_VulkanInstance->Destroy();

			delete m_VulkanInstance;
			m_VulkanInstance = nullptr;
		}
	}

	void Renderer::CreateSurface() {

		m_Surface = new VKSurface();
		m_Surface->Create(m_VulkanInstance, m_Window);
	}

	void Renderer::DestroySurface() {

		if (m_Surface) {

			m_Surface->Destroy();

			delete m_Surface;
			m_Surface = nullptr;
		}
	}

	void Renderer::CreateDevice() {

		m_Device = new VKDevice();
		m_Device->Create(m_VulkanInstance, m_Surface);
	}

	void Renderer::DestroyDevice() {

		if (m_Device) {

			m_Device->Destroy();

			delete m_Device;
			m_Device = nullptr;
		}
	}

	void Renderer::CreateSwapchain() {

		m_Swapchain = new VKSwapchain();
		m_Swapchain->Create(m_Device, m_Surface, m_Window);
	}

	void Renderer::DestroySwapchain() {

		if (m_Swapchain) {

			m_Swapchain->Destroy();
			
			delete m_Swapchain;
			m_Swapchain = nullptr;
		}
	}

	void Renderer::CreateRenderPass() {

		m_RenderPass = new VKRenderPass();
		m_RenderPass->Create(m_Device, m_Swapchain);
	}

	void Renderer::DestroyRenderPass() {

		if (m_RenderPass) {

			m_RenderPass->Destroy();

			delete m_RenderPass;
			m_RenderPass = nullptr;
		}
	}

	void Renderer::CreateFramebuffers() {

		m_Swapchain->CreateSwapchainFramebuffers(m_RenderPass);
	}

	void Renderer::DestroyFramebuffers() {

		if (m_Swapchain) {

			m_Swapchain->DestroySwapchainFramebuffers();
		}
	}

	void Renderer::CreatePipelineLayout() {

		m_Layout = new VKPipelineLayout();
		m_Layout->Create(m_Device);
	}

	void Renderer::DestroyPipelineLayout() {

		if (m_Layout) {

			m_Layout->Destroy();

			delete m_Layout;
			m_Layout = nullptr;
		}
	}

	void Renderer::CreateGraphicsPipeline() {

		m_GraphicsPipeline = new VKGraphicsPipeline();

		LoadShaderResources();
		m_GraphicsPipeline->Create(m_Device, m_Swapchain, m_Layout, m_RenderPass, VKShader::GetVulkanShaderStageInfo());
		DestroyShaderRescources();
	}

	void Renderer::DestroyGraphicsPipeline() {

		if (m_GraphicsPipeline) {

			m_GraphicsPipeline->Destroy();
			
			delete m_GraphicsPipeline;
			m_GraphicsPipeline = nullptr;
		}
	}

	void Renderer::CreateCommands() {

		m_CommandPools.resize(FRAME_LAG);
		m_CommandPools.reserve(FRAME_LAG);

		for (U64 i = 0; i < FRAME_LAG; i++) {

			m_CommandPools[i].Create(m_Device);

			m_CommandBuffers.push_back(std::move(m_CommandPools[i].AllocateCommandBuffers(1)[0]));
		}
	}

	void Renderer::DestroyCommands() {

		for (U64 i = 0; i < FRAME_LAG; i++) {

			m_CommandPools[i].FreeCommandBuffer(m_CommandBuffers[i]);
			m_CommandPools[i].Destroy();
		}

		m_CommandBuffers.clear();
		m_CommandPools.clear();
	}

	void Renderer::CreateSyncronizationPrimitives() {

		m_ImageAvailableSemaphores.resize(FRAME_LAG);
		m_RenderFinishedSemaphores.resize(FRAME_LAG);
		m_CommandBufferExecutedFences.resize(FRAME_LAG);

		for (U64 i = 0; i < FRAME_LAG; i++) {

			m_ImageAvailableSemaphores[i].Create(m_Device);
			m_RenderFinishedSemaphores[i].Create(m_Device);
			m_CommandBufferExecutedFences[i].Create(m_Device);
		}
	}

	void Renderer::DestroySyncronizationPrimitives() {

		for (U64 i = 0; i < FRAME_LAG; i++) {

			m_ImageAvailableSemaphores[i].Destroy();
			m_RenderFinishedSemaphores[i].Destroy();
			m_CommandBufferExecutedFences[i].Destroy();
		}

		m_ImageAvailableSemaphores.clear();
		m_RenderFinishedSemaphores.clear();
		m_CommandBufferExecutedFences.clear();
	}
}