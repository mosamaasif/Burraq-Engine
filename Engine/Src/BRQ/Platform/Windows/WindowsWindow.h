#pragma once

#include "Application/Window.h"
#include "Events/Event.h"

namespace BRQ {

    class WindowsWindow : public Window {

    public:
        using EventCallbackFn = std::function<void(Event&)>;

    private:
        WindowProperties    m_Properties;
        WindowHandle        m_WindowHandle;

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
        
        static LRESULT EventCallbackSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static LRESULT EventCallbackFunction(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT EventCallbackHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);  
    };
}