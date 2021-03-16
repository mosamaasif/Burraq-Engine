#include <BRQ.h>

#include "WindowsWindow.h"
#include "Application/Input.h"

#include "Events/WindowEvents.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace BRQ {

    WindowsWindow::WindowsWindow(const WindowProperties& properties)
        : m_Properties(properties), m_WindowHandle(nullptr) {

        m_Open = Init();
    }

    WindowsWindow::~WindowsWindow() {

        glfwDestroyWindow(m_Window);
        glfwTerminate();

        Input::Shutdown();
    }

    void WindowsWindow::OnUpdate() {

        glfwPollEvents();
    }

    bool WindowsWindow::Init() {

        if (!glfwInit()) {

            return false;
        }
        
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Window = glfwCreateWindow(m_Properties.Width, m_Properties.Height, m_Properties.Title.c_str(), NULL, NULL);

        if (!m_Window) {

            glfwTerminate();
            return false;
        }

        Input::Init(m_Window);

        glfwShowWindow(m_Window);
        glfwSetWindowUserPointer(m_Window, this);

        m_WindowHandle = glfwGetWin32Window(m_Window);

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, I32 width, I32 height) {

                auto win = (WindowsWindow*)glfwGetWindowUserPointer(window);

                win->m_Properties.Width = (U32)width;
                win->m_Properties.Height = (U32)height;
            }
        );

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {

                auto win = (WindowsWindow*)glfwGetWindowUserPointer(window);

                win->m_Open = false;
            }
        );

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, I32 key, I32 scancode, I32 action, I32 mods) {

                auto win = (WindowsWindow*)glfwGetWindowUserPointer(window);
                auto input = Input::GetInstance();

                switch (action) {

                case GLFW_REPEAT:
                case GLFW_PRESS:
                    input->KeyCallback(KeyPressedEvent((Key)key));
                    break;

                case GLFW_RELEASE:
                    input->KeyCallback(KeyReleasedEvent((Key)key));
                    break;
                }
            }
        );

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, I32 button, I32 action, I32 mods) {

                auto win = (WindowsWindow*)glfwGetWindowUserPointer(window);
                auto input = Input::GetInstance();

                switch (action) {

                case GLFW_REPEAT:
                case GLFW_PRESS:
                    input->MouseButtonCallback(MouseButtonPressedEvent((MouseButton)button));
                    break;

                case GLFW_RELEASE:
                    input->MouseButtonCallback(MouseButtonReleasedEvent((MouseButton)button));
                    break;
                }
            }
        );

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, F64 xPos, F64 yPos) {

                auto win = (WindowsWindow*)glfwGetWindowUserPointer(window);
                auto input = Input::GetInstance();

                if (xPos >= 0.0 && xPos <= (F64)win->m_Properties.Width && yPos >= 0.0 && yPos <= (F64)win->m_Properties.Height) {

                    input->MouseMovedCallback(MouseMovedEvent((F32)xPos, (F32)yPos));
                }
                else {

                    input->ClearState();
                }
            }
        );

        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, I32 focused) {

                auto win = (WindowsWindow*)glfwGetWindowUserPointer(window);
                auto input = Input::GetInstance();

                if (focused == -1) {

                    input->ClearState();
                }
            }
        );

        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, I32 width, I32 height) {

                auto win = (WindowsWindow*)glfwGetWindowUserPointer(window);
                
                win->m_Properties.Width = (U32)width;
                win->m_Properties.Height = (U32)height;

                WindowResizeEvent event(win->m_Properties.Width, win->m_Properties.Height);
                win->m_EventCallback(event);
            }
        );
    
        return true;
    }
}