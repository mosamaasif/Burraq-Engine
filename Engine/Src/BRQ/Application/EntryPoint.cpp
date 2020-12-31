#include <BRQ.h>

#include "Platform/Platform.h"
#include "Logger/Log.h"
#include "Window.h"
#include "Input.h"

#include "Platform/Vulkan/VKDevice.h"
#include "Platform/Vulkan/VKSurface.h"
#include "Platform/Vulkan/VKInstance.h"
#include "Platform/Vulkan/VKSwapChain.h"

int main() {

	using namespace BRQ;

	Log::Init();

	Window* window = Window::Create();

	auto input = Input::GetInstance();

	VKInstance instance;
	VKSurface surface;
	VKDevice device;
	VKSwapChain swapchain;

	instance.Create();
	surface.Create(window, &instance);
	device.Create(&instance, &surface);
	swapchain.Create(&surface, &device, window);


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