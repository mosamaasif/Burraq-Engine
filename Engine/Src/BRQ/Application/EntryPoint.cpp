#include <BRQ.h>

#include "Platform/Platform.h"
#include "Logger/Log.h"
#include "Window.h"

int main() {

	BRQ::Log::Init();

	BRQ::Window* window = BRQ::Window::Create();

	while (window->IsOpen()) {

		window->OnUpdate();
	}

	delete window;

	BRQ::Log::Shutdown();

	return 0;
}