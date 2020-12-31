#include <BRQ.h>

#include "Platform/Platform.h"
#include "Logger/Log.h"
#include "Window.h"
#include "Input.h"

#include "Platform/Vulkan/VKDevice.h"
#include "Platform/Vulkan/VKShader.h"
#include "Platform/Vulkan/VKSurface.h"
#include "Platform/Vulkan/VKInstance.h"
#include "Platform/Vulkan/VKSwapChain.h"

int main() {

	using namespace BRQ;
	using namespace Utilities;

	Log::Init();

	Window* window = Window::Create();

	auto input = Input::GetInstance();

	FileSystem::Init();

	auto filesystem = FileSystem::GetInstance();



	VKInstance instance;
	VKSurface surface;
	VKDevice device;
	VKSwapChain swapchain;
	VKShader shader;

	instance.Create();
	surface.Create(window, &instance);
	device.Create(&instance, &surface);
	swapchain.Create(&surface, &device, window);
	shader.Create(&device, "Src/Shaders/vert.spv", VKShader::ShaderType::Vertex);

	while (window->IsOpen()) {

		window->OnUpdate();
	}

	swapchain.Destroy();

	device.Destroy();
	surface.Destroy();
	instance.Destroy();

	delete window;

	Log::Shutdown();

	return 0;
}