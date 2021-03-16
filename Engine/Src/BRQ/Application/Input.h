#pragma once

#include <BRQ.h>

#include "Keys.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#define MAX_KEYS        512
#define MAX_BUTTONS     8

struct GLFWwindow;

namespace BRQ {
    
    class Input {

    private:
        static Input*         s_Instance;

        static GLFWwindow*    s_Window;

        static bool           s_Key[MAX_KEYS];
        static bool           s_Mouse[MAX_BUTTONS];

        static F32            s_MouseX;
        static F32            s_MouseY;

    protected:
        Input() = default;
        Input(const Input& input) = default;

    public:
        ~Input() = default;

        static void Init(GLFWwindow* window);
        static void Shutdown();

        static Input* GetInstance() { return s_Instance; }

        bool IsKeyPressed(Key key);
        bool IsMouseButtonPressed(MouseButton button);

        F32 GetMouseX();
        F32 GetMouseY();
        std::pair<F32, F32> GetMousePosition();
        void SetMousePosition(F32 x, F32 y);

        void ClearState();

        bool KeyCallback(const KeyEvent& event);
        bool MouseButtonCallback(const MouseButtonEvent& event);
        bool MouseMovedCallback(const MouseMovedEvent& event);
    };
}