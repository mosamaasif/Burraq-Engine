#include <BRQ.h>

#include "Window.h"
#include "Platform/Windows/WindowsWindow.h"

namespace BRQ {

	Window::Window() 
		: m_Open(false) { }

	Window* Window::Create(const WindowProperties& properties) {

		#ifdef BRQ_PLATFORM_WINDOWS
			return new WindowsWindow(properties);
		#else
			BRQ_CORE_ASSERT(false, "Unknown platform!");
			return nullptr;
		#endif
	}
}