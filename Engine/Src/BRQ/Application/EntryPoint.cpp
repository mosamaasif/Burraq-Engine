#include <BRQ.h>

#include "Platform/Platform.h"
#include "Logger/Log.h"
#include "Window.h"
#include "Input.h"

int main() {

	BRQ::Log::Init();

	BRQ::Window* window = BRQ::Window::Create();

	auto input = BRQ::Input::GetInstance();

	while (window->IsOpen()) {

		if (input->IsMouseButtonPressed(BRQ::MouseButton::ButtonLeft)) {

			BRQ_WARN("Left");
		}

		if (input->IsMouseButtonPressed(BRQ::MouseButton::ButtonRight)) {

			BRQ_WARN("Right");
		}

		if (input->IsMouseButtonPressed(BRQ::MouseButton::ButtonMiddle)) {

			BRQ_WARN("Middle");
		}

		std::cout << "X: " << input->GetMouseX() << ", Y: " << input->GetMouseY() << std::endl;

		window->OnUpdate();
	}

	delete window;

	BRQ::Log::Shutdown();

	return 0;
}