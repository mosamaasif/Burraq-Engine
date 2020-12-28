#pragma once

#include <BRQ.h>

#include "Keys.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"


#define MAX_KEYS        512
#define MAX_BUTTONS     8

namespace BRQ {
    
    class Input {

    private:
        static Input*   s_Instance;

        static bool     s_Key[MAX_KEYS];
        static bool     s_Mouse[MAX_BUTTONS];

        static F32      s_MouseX;
        static F32      s_MouseY;

    protected:
        Input() = default;
        Input(const Input& input) = default;

    public:
        ~Input() = default;

        static void Init();
        static void Shutdown();

        static Input* GetInstance() { return s_Instance; }

        bool IsKeyPressed(Key key);
        bool IsMouseButtonPressed(MouseButton button);

        F32 GetMouseX();
        F32 GetMouseY();
        std::pair<F32, F32> GetMousePosition();

        void ClearState();

        void KeyCallback(const KeyEvent& event);
        void MouseButtonCallback(const MouseButtonEvent& event);
        void MouseMovedCallback(const MouseMovedEvent& event);
    };
}

