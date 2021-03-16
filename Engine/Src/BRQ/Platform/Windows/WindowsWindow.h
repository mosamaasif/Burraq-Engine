#pragma once

#include "Application/Window.h"
#include "Events/Event.h"

struct GLFWwindow;

namespace BRQ {

    class WindowsWindow : public Window {

    private:
        WindowProperties    m_Properties;
        WindowHandle        m_WindowHandle;
        GLFWwindow*         m_Window;

    public:
        WindowsWindow() = delete;
        WindowsWindow(const WindowProperties& properties);
        ~WindowsWindow();

        void OnUpdate() override;

        U32 GetWidth() const { return m_Properties.Width; }
        U32 GetHeight() const { return m_Properties.Height; }

        WindowHandle GetNativeWindowHandle() const { return m_WindowHandle; };

    private:
        bool Init();
    };
}