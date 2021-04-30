#pragma once

#include "Input.h"
#include "Window.h"
#include "Events/Event.h"
#include "Utilities/Timer.h"
#include "Graphics/Renderer.h"
#include "Events/WindowEvents.h"
#include "Camera/CameraController.h"

namespace BRQ {

    class Application {

    private:
        Application*     s_Application;

        WindowProperties m_WindowProperties;
        Window*	         m_Window;

        bool             m_Minimized;

    protected:
        Renderer*        m_Renderer;
        CameraController m_CameraController;
        Input*           m_InputManager;

    protected:
        Application();
        Application(const WindowProperties& props = WindowProperties());

    public:
        virtual ~Application();

        void Run();
        virtual void OnUpdate(F32 dt) = 0;
        void OnEvent(Event& event);

        Application* GetApplication() { return s_Application; }

    private:
        void OnUpdateInternal(F32 dt);
        bool OnWindowResize(WindowResizeEvent& event);
    };

    Application* CreateApplication(const WindowProperties& props = WindowProperties());
}