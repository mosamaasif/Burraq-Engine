#include <BRQ.h>
#include "CameraController.h"
#include "Application/Input.h"

namespace BRQ {

    CameraController::CameraController(U32 width, U32 height, F32 fov, bool capture) {
    
        m_AspectRatio = (F32)width / (F32)height;
        m_Fov = fov;

        CaptureCamera(capture);

        m_Width = (F32)width;
        m_Height = (F32)height;

        glm::mat4 projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, 0.1f, 1000.0f);
        m_Camera = Camera(projection, width, height);
    }

    void CameraController::OnUpdate(float dt) {

        auto input = Input::GetInstance();

        glm::vec2 pos = { input->GetMouseX(), input->GetMouseY() };

        F32 deltaX = pos.x - (I32)m_Camera.m_WindowCenterX;
        F32 deltaY = (I32)m_Camera.m_WindowCenterY - pos.y;

        F64 yaw = (F64)deltaX * (F64)m_Camera.m_MouseSensitivity;
        F64 pitch = (F64)deltaY * (F64)m_Camera.m_MouseSensitivity;

        m_Camera.m_Yaw += (F32)(yaw);
        m_Camera.m_Pitch += (F32)(pitch);

        // Avoids Gimbal lock
        if (m_Camera.m_Pitch > 89.0f)
            m_Camera.m_Pitch = 89.0f;
        if (m_Camera.m_Pitch < -89.0f)
            m_Camera.m_Pitch = -89.0f;

        F32 velocity = m_Camera.m_Speed * dt;

        if (input->IsKeyPressed(Key::KEY_LSHIFT))
            velocity = m_Camera.m_Speed * dt * 6.0f;
        if (input->IsKeyPressed(Key::KEY_W))
            m_Camera.m_Position += m_Camera.m_Front * velocity;
        if (input->IsKeyPressed(Key::KEY_S))
            m_Camera.m_Position -= m_Camera.m_Front * velocity;
        if (input->IsKeyPressed(Key::KEY_A))
            m_Camera.m_Position -= m_Camera.m_Right * velocity;
        if (input->IsKeyPressed(Key::KEY_D))
            m_Camera.m_Position += m_Camera.m_Right * velocity;
        if (input->IsKeyPressed(Key::KEY_SPACE))
            m_Camera.m_Position += m_Camera.m_Up * velocity;
        if (input->IsKeyPressed(Key::KEY_LCONTROL))
            m_Camera.m_Position -= m_Camera.m_Up * velocity;

        if (m_CaptureCamera) {

            input->SetMousePosition(m_Camera.m_WindowCenterX, m_Camera.m_WindowCenterY);
            m_Camera.Update();
        }
    }

    void CameraController::OnEvent(Event& event) {

        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) {

                return OnWindowResized(event);
            }
        );
    }

    void CameraController::CaptureCamera(bool capture) {

        auto input = Input::GetInstance();

        input->SetInputMode(Input::InputMode::CursorHidden);

        if (capture) {

            input->SetInputMode(Input::InputMode::CursorHidden);
        }
        else {

            input->SetInputMode(Input::InputMode::CursorNormal);
        }

        m_CaptureCamera = capture;
    }

    void CameraController::Reset() {

        glm::mat4 projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, 0.1f, 1000.0f);
        m_Camera = Camera(projection, (U32)m_Width, (U32)m_Height);
        m_Camera.Update();
    }

    bool CameraController::OnWindowResized(WindowResizeEvent& event) {

        U32 width = event.GetWidth();
        U32 height = event.GetHeight();

        if (width == 0 || height == 0) {

            return false;
        }

        m_Width = (F32)width;
        m_Height = (F32)height;

        m_AspectRatio = m_Width / m_Height;

        glm::mat4 projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, 0.1f, 1000.0f);
        m_Camera = Camera(projection, (U32)m_Width, (U32)m_Height);

        m_Camera.m_WindowCenterX = m_Width / 2.0f;
        m_Camera.m_WindowCenterY = m_Height / 2.0f;
        m_Camera.Update();

        return false;
    }
}