#include <BRQ.h>

#include "Application.h"

namespace BRQ {

    Application::Application() {

        BRQ_ASSERT("Default Application Constructor");
    }

    Application::Application(const WindowProperties& props)
        : m_Window(nullptr), m_Renderer(nullptr), m_Minimized(true) {
        
        BRQ_ASSERT("Default Application Constructor");

        Log::Init();
        Utilities::FileSystem::Init();

        m_Window = Window::Create(m_WindowProperties = props);
        m_Window->SetEventCallbackFunction(BRQ_BIND_EVENT_FN(OnEvent));

        // TODO: Defer Pipeline Building. This is temporary
        Renderer::SubmitResources({ {"Shaders/shader.vert.spv", VKShader::ShaderType::Vertex},
                                    {"Shaders/shader.frag.spv", VKShader::ShaderType::Fragment} });
        Renderer::Init(m_Window);
        m_Renderer = Renderer::GetInstance();

        m_InputManager = Input::GetInstance();

        m_CameraController = CameraController(m_Window->GetWidth(), m_Window->GetHeight(), 60.0f);
        m_Minimized = false;
    }

    Application::~Application() {

        Renderer::Shutdown();
        m_Renderer = nullptr;

        delete m_Window;
        m_Window = nullptr;

        Utilities::FileSystem::Shutdown();
        Log::Shutdown();
    }

    void Application::OnEvent(Event& event) {

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowResizeEvent>(BRQ_BIND_EVENT_FN(Application::OnWindowResize));
    }

    void Application::Run() {

        Timer clock;

        F32 start = 0.0f;
        F32 end = 0.0f;

        while (m_Window->IsOpen()) {

            start = end;
            end = clock.GetTime();

            F32 dt = end - start;

            if (!m_Minimized) {

                OnUpdate(dt);
            }

            m_Window->OnUpdate();
        }
    }

    void Application::OnUpdate(F32 dt) {

        m_CameraController.OnUpdate(dt);

        if (m_InputManager->IsKeyPressed(Key::KEY_T)) {

            m_CameraController.CaptureCamera(true);
        }
        else if (m_InputManager->IsKeyPressed(Key::KEY_Y)) {

            m_CameraController.CaptureCamera(false);
        }

        if (m_InputManager->IsKeyPressed(Key::KEY_R)) {

            m_CameraController.Reset();
        }

        if (!m_Minimized) {

            m_Renderer->BeginScene(m_CameraController.GetCamera());

            // draw here

            m_Renderer->EndScene();

            m_Renderer->Present();
        }
    }
    
    bool Application::OnWindowResize(WindowResizeEvent& event) {

        m_CameraController.OnEvent(event);

        if (event.GetHeight() == 0 || event.GetWidth() == 0) {

            m_Minimized = true;
        }
        else {

            m_Minimized = false;
        }

        return false;
    }
}