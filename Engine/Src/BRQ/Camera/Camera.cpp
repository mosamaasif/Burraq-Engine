#include <BRQ.h>
#include "Camera.h"

namespace BRQ {

    Camera::Camera(const glm::mat4& projectionMatrix, U32 width, U32 height) {

        m_ProjectionMatrix = projectionMatrix;
        m_ViewMatrix = glm::mat4(1.0f);

        m_Position = { 0.0f, 0.0f, 2.0f };
        //m_Front = { 0.00414f, 0.99985f, 0.01695f };
        m_Front = { 0.0f, 0.0f, 0.0f };
        m_WorldUp = { 0.0f, 1.0f, 0.0f };

        m_WindowCenterX = (F32)width / 2.0f;
        m_WindowCenterY = (F32)height / 2.0f;

        m_Yaw = -90.f;
        m_Pitch = 0.0f;
        m_Speed = 0.001f;
        m_MouseSensitivity = 0.007f;
    }

    void Camera::Update() {

        CalculateVectors();

        m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    }

    void Camera::CalculateVectors() {

        F32 x = (F32)glm::cos(glm::radians(m_Yaw)) * (F32)glm::cos(glm::radians(m_Pitch));
        F32 y = (F32)glm::sin(glm::radians(m_Pitch));
        F32 z = (F32)glm::sin(glm::radians(m_Yaw)) * (F32)glm::cos(glm::radians(m_Pitch));

        m_Front.x = x;
        m_Front.y = y;
        m_Front.z = z;

        m_Front = glm::normalize(m_Front);

        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }
}