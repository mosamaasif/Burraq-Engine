#include <BRQ.h>

#include "WindowsWindow.h"

namespace BRQ {

    WindowsWindow::WindowsWindow(const WindowProperties& properties)
        : m_Properties(properties), m_WindowHandle(nullptr) {

        m_Open = Init();
    }

    WindowsWindow::~WindowsWindow() {

        DestroyWindow((HWND)m_WindowHandle);
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

        LPCWSTR className = L"RTX Window Class";

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

        m_WindowHandle = CreateWindowEx(
            WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
            className,
            title.c_str(),
            WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            CW_USEDEFAULT, CW_USEDEFAULT, m_Properties.Width, m_Properties.Height,
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

        switch (msg) {

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }
}