#include <BRQ.h>

#include "Application.h"



namespace BRQ {

    Application::Application() {

        BRQ_ASSERT("Default Application Constructor");
    }

    Application::Application(const WindowProperties& props)
        : m_Window(nullptr), m_Renderer(nullptr), m_Running(false), m_Minimized(true) {
        
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

        m_Running = true;
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
        dispatcher.Dispatch<WindowCloseEvent>(BRQ_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BRQ_BIND_EVENT_FN(Application::OnWindowResize));
    }

    void Application::Run() {

        // temp
        while (m_Window->IsOpen()) {

            if (!m_Minimized) {

                m_Renderer->Present();
            }

            m_Window->OnUpdate();
        }
    }
    
    bool Application::OnWindowResize(WindowResizeEvent& event) {

        if (event.GetHeight() == 0 || event.GetWidth() == 0) {

            m_Minimized = true;
        }
        else {

            m_Minimized = false;
        }

        return false;
    }

    bool Application::OnWindowClose(WindowCloseEvent& event) {

        m_Running = false;
        return false;
    }

}