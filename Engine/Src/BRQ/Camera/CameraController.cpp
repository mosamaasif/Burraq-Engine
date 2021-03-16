#include <BRQ.h>
#include "CameraController.h"
#include "Application/Input.h"

namespace BRQ {

	CameraController::CameraController(U32 width, U32 height, F32 fov, bool capture) {
	
		m_AspectRatio = (F32)width / (F32)height;
		m_Fov = fov;
		m_CaptureCamera = capture;

		glm::mat4 projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, 0.1f, 1000.0f);
		m_Camera = Camera(projection, width, height);
	}

	void CameraController::OnUpdate(float dt) {

		auto input = Input::GetInstance();

		glm::vec2 pos = { input->GetMouseX(), input->GetMouseY() };

		float deltaX = pos.x - m_Camera.m_LastMousePosition.x;
		float deltaY = m_Camera.m_LastMousePosition.y - pos.y;

		if (m_CaptureCamera) {

			input->SetMousePosition(m_Camera.m_LastMousePosition.x, m_Camera.m_LastMousePosition.y);
		}

		m_Camera.m_Yaw += deltaX * m_Camera.m_MouseSensitivity * 0.3;
		m_Camera.m_Pitch += deltaY * m_Camera.m_MouseSensitivity * 0.3;

		// Avoids Gimbal lock
		if (m_Camera.m_Pitch > 89.0f)
			m_Camera.m_Pitch = 89.0f;
		if (m_Camera.m_Pitch < -89.0f)
			m_Camera.m_Pitch = -89.0f;

		float velocity = m_Camera.m_Speed * dt * 0.1;

		if (input->IsKeyPressed(Key::KEY_W))
			m_Camera.m_Position += m_Camera.m_Front * velocity;
		if (input->IsKeyPressed(Key::KEY_S))
			m_Camera.m_Position -= m_Camera.m_Front * velocity;
		if (input->IsKeyPressed(Key::KEY_A))
			m_Camera.m_Position -= m_Camera.m_Right * velocity;
		if (input->IsKeyPressed(Key::KEY_D))
			m_Camera.m_Position += m_Camera.m_Right * velocity;
		if (input->IsKeyPressed(Key::KEY_LSHIFT))
			m_Camera.m_Position += m_Camera.m_Up * velocity;
		if (input->IsKeyPressed(Key::KEY_LCONTROL))
			m_Camera.m_Position -= m_Camera.m_Up * velocity;

		m_Camera.Update();
	}

	void CameraController::OnEvent(Event& event) {

		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) {

			return OnWindowResized(event);
			});
	}

	void CameraController::CaptureCamera(bool capture) {

		m_CaptureCamera = capture;
	}

	void CameraController::Reset() {

		m_Camera.m_Front = { 0.00414f, 0.99985f, 0.01695f };
		m_Camera.Update();
	}

	bool CameraController::OnWindowResized(WindowResizeEvent& event) {

		F32 width = (F32)event.GetWidth();
		F32 height = (F32)event.GetHeight();

		m_Camera.m_LastMousePosition = { width / 2.0f, height / 2.0f };

		m_AspectRatio = width / height;

		glm::mat4 projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, 0.1f, 1000.0f);
		m_Camera = Camera(projection, width, height);
		m_Camera.Update();

		return false;
	}
}