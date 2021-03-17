#pragma once

#include "Camera.h"
#include "Events/WindowEvents.h"

namespace BRQ {

	class CameraController {

	private:
		F32		m_Fov;
		F32		m_AspectRatio;

		F32		m_Width;
		F32		m_Height;

		Camera	m_Camera;
		bool	m_CaptureCamera;

	public:
		CameraController() = default;
		CameraController(U32 width, U32 height, F32 fov, bool captureCamera = true);
		~CameraController() = default;

		void OnUpdate(F32 dt);
		void OnEvent(Event& event);

		void CaptureCamera(bool capture);

		void Reset();

		const Camera& GetCamera() const { return m_Camera; }

	private:
		bool OnWindowResized(WindowResizeEvent& event);
	};
}