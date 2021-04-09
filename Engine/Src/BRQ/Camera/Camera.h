#pragma once

#include "Math/Math.h"
#include "Utilities/Types.h"

namespace BRQ {

    class Camera {

        friend class CameraController;

    private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;

        glm::vec3 m_Position;
        glm::vec3 m_Front;
        glm::vec3 m_Right;
        glm::vec3 m_Up;
        glm::vec3 m_WorldUp;

        F32	      m_WindowCenterX;
        F32       m_WindowCenterY;

        F32	      m_Yaw;
        F32       m_Pitch;
        F32       m_Speed;
        F32       m_MouseSensitivity;

    public:
        Camera() = default;
        Camera(const glm::mat4& projectionMatrix, U32 width, U32 height);
        ~Camera() = default;

        void Update();

        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

    private:
        void CalculateVectors();
    };
}