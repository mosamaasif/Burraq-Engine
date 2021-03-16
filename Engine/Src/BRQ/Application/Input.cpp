#include "BRQ.h"
#include "Input.h"
#include "Events/MouseEvent.h"
#include <GLFW/glfw3.h>

namespace BRQ {

    Input*      Input::s_Instance = nullptr;
    GLFWwindow* Input::s_Window = nullptr;
    bool        Input::s_Key[MAX_KEYS];
    bool        Input::s_Mouse[MAX_BUTTONS];
    F32         Input::s_MouseX = 0.0f;
    F32         Input::s_MouseY = 0.0f;

    void Input::Init(GLFWwindow* window) {

        s_Instance = new Input();

        s_Window = window;

        memset(s_Key, 0, MAX_KEYS);
        memset(s_Mouse, 0, MAX_BUTTONS);
    }

    void Input::Shutdown() {

        if (s_Instance)
            delete[] s_Instance;
    }

    bool Input::IsKeyPressed(Key key) {

        return s_Key[(U16)key];
    }

    bool Input::IsMouseButtonPressed(MouseButton button) {

        return s_Mouse[(U16)button];
    }

    float BRQ::Input::GetMouseX() {

        return s_MouseX;
    }

    float BRQ::Input::GetMouseY() {

        return s_MouseY;
    }

    std::pair<F32, F32> BRQ::Input::GetMousePosition() {

        return std::pair<F32, F32>(s_MouseX, s_MouseY);
    }

    void Input::SetMousePosition(F32 x, F32 y) {

        glfwSetCursorPos(s_Window, x, y);
    }

    void Input::ClearState() {

        memset(s_Key, 0, MAX_KEYS);
        memset(s_Mouse, 0, MAX_BUTTONS);
        s_MouseX = 0.0f;
        s_MouseY = 0.0f;
    }

    bool Input::KeyCallback(const KeyEvent& event) {

        Key key = event.GetKeyCode();
        EventType type = event.GetEventType();

        if (type == EventType::KeyPressed) {

            s_Key[(U16)key] = true;
        }
        else if (type == EventType::KeyReleased) {

            s_Key[(U16)key] = false;
        }
        return true;
    }

    bool Input::MouseButtonCallback(const MouseButtonEvent& event) {

        MouseButton button = event.GetMouseButton();
        EventType type = event.GetEventType();

        if (type == EventType::MouseButtonPressed) {

            s_Mouse[(U16)button] = true;
        }
        else if (type == EventType::MouseButtonReleased) {

            s_Mouse[(U16)button] = false;
        }
        return true;
    }

    bool Input::MouseMovedCallback(const MouseMovedEvent& event) {

        EventType type = event.GetEventType();

        if (type == EventType::MouseMoved) {

            s_MouseX = event.GetX();
            s_MouseY = event.GetY();
        }
        return true;
    }
}