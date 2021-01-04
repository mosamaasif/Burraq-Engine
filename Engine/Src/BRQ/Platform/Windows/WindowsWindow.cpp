#include <BRQ.h>

#include "WindowsWindow.h"
#include "Application/Input.h"

#include "Events/WindowEvents.h"

namespace BRQ {

    WindowsWindow::WindowsWindow(const WindowProperties& properties)
        : m_Properties(properties), m_WindowHandle(nullptr) {

        Input::Init();
        m_Open = Init();
    }

    WindowsWindow::~WindowsWindow() {

        DestroyWindow((HWND)m_WindowHandle);
        Input::Shutdown();
    }

    void WindowsWindow::OnUpdate() {

        MSG message;
        while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE) > 0) {

            if (message.message == WM_QUIT) {

                m_Open = false;
                return;
            }
            TranslateMessage(&message);
            DispatchMessage(&message);

        }
    }

    bool WindowsWindow::Init() {

        LPCWSTR className = L"Burraq Window Class";

        WNDCLASSEX wc = {};
        ZeroMemory(&wc, sizeof(WNDCLASSEX));

        HINSTANCE hInstance = GetModuleHandle(NULL);

        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = 0;
        wc.lpfnWndProc = EventCallbackSetup;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
        wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = className;
        wc.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

        if (!RegisterClassEx(&wc)) {

            BRQ_CORE_ASSERT("Window Class Registration Failed!");
            return false;
        }

        std::wstring title(m_Properties.Title.begin(), m_Properties.Title.end());

        RECT r = { 0, 0, (LONG)m_Properties.Width, (LONG)m_Properties.Height };

        AdjustWindowRectEx(&r, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, false, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);

        m_WindowHandle = CreateWindowEx(
            WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
            className,
            title.c_str(),
            WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
            NULL, NULL, hInstance, this);

        if (m_WindowHandle == NULL) {

            BRQ_CORE_ASSERT("Window Creation Failed!");
            return false;
        }

        ShowWindow((HWND)m_WindowHandle, SW_SHOWDEFAULT);
        SetFocus((HWND)m_WindowHandle);

        return true;
    }
    
    LRESULT WindowsWindow::EventCallbackSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

        if (msg == WM_NCCREATE) {

            const CREATESTRUCTW* create = (CREATESTRUCTW*)(lParam);
            WindowsWindow* const window = (WindowsWindow*)(create->lpCreateParams);

            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(window));
            SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)(&WindowsWindow::EventCallbackFunction));

            return window->EventCallbackHandler(hwnd, msg, wParam, lParam);
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT WindowsWindow::EventCallbackFunction(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

        WindowsWindow* const window = (WindowsWindow*)(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        return window->EventCallbackHandler(hwnd, msg, wParam, lParam);
    }

    LRESULT WindowsWindow::EventCallbackHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

        auto input = Input::GetInstance();

        switch (msg) {

        case WM_DESTROY:

            PostQuitMessage(0);
            break;

        case WM_KILLFOCUS:

            input->ClearState();
            break;

        case WM_KEYDOWN:

            input->KeyCallback(KeyPressedEvent((Key)wParam));
            break;

        case WM_KEYUP:

            input->KeyCallback(KeyPressedEvent((Key)wParam));
            break;

        case WM_LBUTTONDOWN:

            SetCapture((HWND)m_WindowHandle);
            input->MouseButtonCallback(MouseButtonPressedEvent(MouseButton::ButtonLeft));
            break;

        case WM_LBUTTONUP:

            ReleaseCapture();
            input->MouseButtonCallback(MouseButtonReleasedEvent(MouseButton::ButtonLeft));
            break;

        case WM_RBUTTONDOWN:

            SetCapture((HWND)m_WindowHandle);
            input->MouseButtonCallback(MouseButtonPressedEvent(MouseButton::ButtonRight));
            break;

        case WM_RBUTTONUP:

            ReleaseCapture();
            input->MouseButtonCallback(MouseButtonReleasedEvent(MouseButton::ButtonRight));
            break;

        case WM_MBUTTONDOWN:

            SetCapture((HWND)m_WindowHandle);
            input->MouseButtonCallback(MouseButtonPressedEvent(MouseButton::ButtonMiddle));
            break;

        case WM_MBUTTONUP:

            ReleaseCapture();
            input->MouseButtonCallback(MouseButtonReleasedEvent(MouseButton::ButtonMiddle));
            break;

        case WM_MOUSEMOVE:

            POINTS pt = MAKEPOINTS(lParam);

            if (pt.x >= 0 && pt.x <= (SHORT)m_Properties.Width && pt.y >= 0 && pt.y <= (SHORT)m_Properties.Height) {

                input->MouseMovedCallback(MouseMovedEvent(pt.x, pt.y));
                SetCapture((HWND)m_WindowHandle);
            }
            else {

                ReleaseCapture();
                input->ClearState();
            }
            break;

        case WM_SIZE:

            m_Properties.Width = LOWORD(lParam);
            m_Properties.Height = HIWORD(lParam);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }
}