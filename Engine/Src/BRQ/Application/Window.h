#pragma once

#include <BRQ.h>
#include "Events/Event.h"

struct GLFWwindow;

namespace BRQ {

    struct WindowProperties {

        std::string Title;
        U32         Width;
        U32         Height;

        WindowProperties(std::string title = "Burraq Engine", U32 width = 1280U, U32 height = 720U)
            : Title(title), Width(width), Height(height) { }
    };

    class Window  {

    public:
        typedef void* WindowHandle;
        using EventCallbackFunction = std::function<void(Event&)>;

    private:
        WindowProperties      m_Properties;
        WindowHandle          m_WindowHandle;
        GLFWwindow*           m_Window;
        EventCallbackFunction m_EventCallback;
        bool                  m_Open;

    public:
        Window() = delete;
        Window(const WindowProperties& properties);
        ~Window();

        void OnUpdate();

        U32 GetWidth() const { return m_Properties.Width; }
        U32 GetHeight() const { return m_Properties.Height; }

        const std::string& GetWindowTitle() const { return m_Properties.Title; }

        WindowHandle GetNativeWindowHandle() const { return m_WindowHandle; };

        void SetEventCallbackFunction(const EventCallbackFunction& function) { m_EventCallback = function; }
        bool IsOpen() const { return m_Open; };

    private:
        bool Init();
    };
}