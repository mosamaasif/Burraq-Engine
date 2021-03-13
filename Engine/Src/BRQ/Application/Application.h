#pragma once

#include "Window.h"
#include "Events/Event.h"
#include "Graphics/Renderer.h"

#include "Events/WindowEvents.h"

namespace BRQ {

    class Application {

    private:
        Application*		s_Application;

        WindowProperties	m_WindowProperties;
        Window*				m_Window;

        bool				m_Running;
        bool				m_Minimized;

    protected:
        Renderer*			m_Renderer;

    protected:
        Application();
        Application(const WindowProperties& props = WindowProperties());

    public:
        virtual ~Application();

        void Run();
        void OnEvent(Event& event);

        Application* GetApplication() { return s_Application; }

    private:
        bool OnWindowResize(WindowResizeEvent& event);
        bool OnWindowClose(WindowCloseEvent& event);
    };

    Application* CreateApplication(const WindowProperties& props = WindowProperties());
}