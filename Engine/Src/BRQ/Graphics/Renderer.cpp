#include <BRQ.h>

#include "Renderer.h"

#include "Application/Window.h"
#include "Utilities/VulkanMemoryAllocator.h"

#include "Platform/Vulkan/VKIndexBuffer.h"
#include "Platform/Vulkan/VKVertexBuffer.h"
#include "Graphics/Mesh.h"



namespace BRQ {

	// this should be here
	Mesh mesh;

	VKVertexBuffer buffer;
	VKIndexBuffer indexBuffer;

	Renderer* Renderer::s_Renderer = nullptr;
	std::vector<std::pair<std::string, VKShader::ShaderType>> Renderer::s_ShaderResources;

	Renderer::Renderer()
		: m_VulkanInstance(nullptr), m_Surface(nullptr), m_Device(nullptr),
		m_Swapchain(nullptr), m_RenderPass(nullptr), m_Layout(nullptr),
		m_GraphicsPipeline(nullptr), m_CommandPool(nullptr), m_Window(nullptr) { }

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

		U32 currentIndex = m_Swapchain->GetCurrentImageIndex();

		m_InFlightFences[currentIndex].Wait();

		m_Swapchain->AcquireNextImageIndex(&m_ImageAvailableSemaphores[currentIndex]);
		U32 acquiredIndex = m_Swapchain->GetAcquiredNextImageIndex();

		m_ImagesInFlightFences[acquiredIndex].Wait();

		if (m_Swapchain->GetSwapchainStatus() == VKSwapchain::SwapchainStatus::NotReady) {

			RecreateSwapchain();
			// resubmitting command buffer lol
			// Pls dont do this for gods sake
			for (U64 i = 0; i < m_CommandBuffers.size(); i++) {

				m_CommandBuffers[i].Begin();
				m_RenderPass->Begin(&m_CommandBuffers[i], &m_Swapchain->GetFramebuffers()[i]);
				m_GraphicsPipeline->Bind(&m_CommandBuffers[i]);


				VkDeviceSize offset = 0;
				vkCmdBindVertexBuffers(m_CommandBuffers[i].GetCommandBuffer(), 0, 1, &buffer.GetVertexBuffer(), &offset);
				vkCmdBindIndexBuffer(m_CommandBuffers[i].GetCommandBuffer(), indexBuffer.GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

				vkCmdDrawIndexed(m_CommandBuffers[i].GetCommandBuffer(), mesh.GetIndices().size(), 1, 0, 0, 0);

				m_RenderPass->End(&m_CommandBuffers[i]);
				m_CommandBuffers[i].End();
			}

			return;
		}

		m_ImagesInFlightFences[acquiredIndex] = m_InFlightFences[currentIndex];

		m_InFlightFences[currentIndex].Reset();

		m_CommandBuffers[acquiredIndex].Submit(m_Device, &m_ImageAvailableSemaphores[currentIndex], &m_RenderFinishedSemaphores[currentIndex], &m_InFlightFences[currentIndex]);

		m_Swapchain->Present(&m_RenderFinishedSemaphores[currentIndex]);
	}

	void Renderer::InitInternal(const Window* window) {

		m_VulkanInstance = new VKInstance();
		m_Surface = new VKSurface();
		m_Device = new VKDevice();
		m_Swapchain = new VKSwapchain();
		m_RenderPass = new VKRenderPass();
		m_Layout = new VKPipelineLayout();
		m_GraphicsPipeline = new VKGraphicsPipeline();
		m_CommandPool = new VKCommandPool();
		m_Window = window;

		m_VulkanInstance->Create();
		m_Surface->Create(m_VulkanInstance, m_Window);
		m_Device->Create(m_VulkanInstance, m_Surface);
		m_Swapchain->Create(m_Device, m_Surface, m_Window);
		m_RenderPass->Create(m_Device, m_Swapchain);
		m_Layout->Create(m_Device);
		
		LoadShaderResources();
		m_GraphicsPipeline->Create(m_Device, m_Swapchain, m_Layout, m_RenderPass, VKShader::GetVulkanShaderStageInfo());
		DestroyShaderRescources();

		m_Swapchain->CreateFramebuffers(m_RenderPass);
		m_CommandPool->Create(m_Device);

		m_CommandBuffers = m_CommandPool->AllocateCommandBuffers(m_Swapchain->GetFramebuffers().size());

		m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		m_ImagesInFlightFences.resize(m_Swapchain->GetSwapchainImage().size());

		for (U64 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

			m_ImageAvailableSemaphores[i].Create(m_Device);
			m_RenderFinishedSemaphores[i].Create(m_Device);
			m_InFlightFences[i].Create(m_Device);
		}


		VulkanMemoryAllocator::Init(m_VulkanInstance, m_Device);

		//mesh.LoadMesh("Src/Models/kitten.obj");
		//mesh.LoadMesh("Src/Models/Mickeytest.obj");
		mesh.LoadMesh("Src/Models/pirate.obj");
		buffer.Create(mesh.GetVertices());
		indexBuffer.Create(mesh.GetIndices());
		

		for (U64 i = 0; i < m_CommandBuffers.size(); i++) {

			m_CommandBuffers[i].Begin();
			m_RenderPass->Begin(&m_CommandBuffers[i], &m_Swapchain->GetFramebuffers()[i]);
			m_GraphicsPipeline->Bind(&m_CommandBuffers[i]);


			VkDeviceSize offset = 0;
			vkCmdBindVertexBuffers(m_CommandBuffers[i].GetCommandBuffer(), 0, 1, &buffer.GetVertexBuffer(), &offset);
			vkCmdBindIndexBuffer(m_CommandBuffers[i].GetCommandBuffer(), indexBuffer.GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(m_CommandBuffers[i].GetCommandBuffer(), mesh.GetIndices().size(), 1, 0, 0, 0);

			m_RenderPass->End(&m_CommandBuffers[i]);
			m_CommandBuffers[i].End();
		}
	}

	void Renderer::DestroyInternal() {

		m_Device->WaitDeviceIdle();

		indexBuffer.Destroy();
		buffer.Destroy();

		VulkanMemoryAllocator::Shutdown();

		for (U64 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

			m_ImageAvailableSemaphores[i].Destroy();
			m_RenderFinishedSemaphores[i].Destroy();
			m_InFlightFences[i].Destroy();
		}

		m_CommandPool->Destroy();
		m_Swapchain->DestoryFramebuffers();
		m_GraphicsPipeline->Destroy();
		m_Layout->Destroy();
		m_RenderPass->Destroy();
		m_Swapchain->Destroy();
		m_Device->Destroy();
		m_Surface->Destroy();
		m_VulkanInstance->Destroy();

		delete m_CommandPool;
		delete m_GraphicsPipeline;
		delete m_Layout;
		delete m_RenderPass;
		delete m_Swapchain;
		delete m_Device;
		delete m_Surface;
		delete m_VulkanInstance;
	}

	void Renderer::RecreateSwapchain() {

		U32 width  = 0;
		U32 height = 0;

		width = m_Window->GetWidth();
		height = m_Window->GetHeight();

		if (width == 0 || height == 0)
			return;
		
		m_Device->WaitDeviceIdle();

		m_Swapchain->DestoryFramebuffers();
		m_CommandPool->FreeCommandBuffers(m_CommandBuffers);
		m_GraphicsPipeline->Destroy();
		m_Layout->Destroy();
		m_RenderPass->Destroy();
		m_Swapchain->Destroy();

		m_Swapchain->Create(m_Device, m_Surface, m_Window);
		m_RenderPass->Create(m_Device, m_Swapchain);
		m_Layout->Create(m_Device);
		LoadShaderResources();
		m_GraphicsPipeline->Create(m_Device, m_Swapchain, m_Layout, m_RenderPass, VKShader::GetVulkanShaderStageInfo());
		DestroyShaderRescources();
		m_Swapchain->CreateFramebuffers(m_RenderPass);
		m_CommandBuffers = m_CommandPool->AllocateCommandBuffers(m_Swapchain->GetFramebuffers().size());
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
}