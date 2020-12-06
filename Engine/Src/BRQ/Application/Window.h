#pragma once

#include <BRQ.h>

namespace BRQ {

    struct WindowProperties {

        std::string Title;
        U32         Width;
        U32         Height;

        WindowProperties(std::string title = "Burraq Engine", U32 width = 1280U, U32 height = 720U)
            : Title(title), Width(width), Height(height) { }
    };

    class Window {

	public:
		typedef void* WindowHandle;

	protected:
		bool m_Open;

	protected:
		Window();

	public:
		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual U32 GetWidth() const = 0;
		virtual U32 GetHeight() const = 0;

		virtual WindowHandle GetNativeWindowHandle() const = 0;

		bool IsOpen() const { return m_Open; }

		static Window* Create(const WindowProperties& properties = WindowProperties());
    };
}