#include "Window/Window.h"

namespace library
{
    LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        Window* pThis = nullptr;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            pThis = reinterpret_cast<Window*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
            pThis->m_hWnd = hWnd;
        }
        else
        {
            pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if (pThis)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    Window::Window()
        :m_hInstance(nullptr)
        ,m_hWnd(nullptr)
        ,m_pszWindowName(PSZ_TITLE)
    {
    }

	HRESULT Window::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow, _In_ PCWSTR pszWindowName)
	{
		HRESULT hr = S_OK;

		// Register the window class
        WNDCLASSEX wcex = {
            .cbSize = sizeof(WNDCLASSEX),
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = WndProc,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = hInstance,
            .hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ICON1),
            .hCursor = LoadCursor(nullptr, IDC_ARROW),
            .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
            .lpszMenuName = nullptr,
            .lpszClassName = GetWindowClassName(),
            .hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON1)
        };

        if (!RegisterClassEx(&wcex))
        {
            DWORD dwError = GetLastError();
            MessageBox(
                nullptr,
                L"Call to RegisterClassEx failed!",
                L"D3D12_DXR",
                NULL
            );
            if (dwError != ERROR_CLASS_ALREADY_EXISTS)
            {
                return HRESULT_FROM_WIN32(dwError);
            }
            return E_FAIL;
        }

        // Creates the window
        m_hInstance = hInstance;

        m_hWnd = CreateWindowEx(
            0,
            GetWindowClassName(),
            m_pszWindowName,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            800,
            600,
            NULL,
            NULL,
            m_hInstance,
            this
        );

        if (!m_hWnd)
        {
            DWORD dwError = GetLastError();
            MessageBox(
                nullptr,
                L"Call to CreateWindow failed!",
                L"D3D12_DXR",
                NULL
            );
            if (dwError != ERROR_CLASS_ALREADY_EXISTS)
            {
                return HRESULT_FROM_WIN32(dwError);
            }
            return E_FAIL;
        }

        // show Window
        ShowWindow(m_hWnd, nCmdShow);

		return S_OK;
	}

    LRESULT Window::HandleMessage(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CLOSE:
            DestroyWindow(m_hWnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }
        return 0;
    }

    PCWSTR Window::GetWindowClassName() const
    {
        return L"Window Class";
    }

    HWND Window::GetWindow() const
    {
        return m_hWnd;
    }
}