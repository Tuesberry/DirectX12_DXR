#include "Game.h"

namespace library
{
	Game::Game(_In_ PCWSTR pszGameName)
		: m_pszGameName(pszGameName)
		, m_Window(std::make_unique<MainWindow>())
		, m_renderer(std::make_unique<Renderer>())
	{
	}

	HRESULT Game::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow)
	{
		if (FAILED(m_Window->Initialize(hInstance, nCmdShow, m_pszGameName)))
		{
			MessageBox(
				nullptr,
				L"Call to Window Initialize failed!",
				L"D3D12_DXR",
				NULL
			);
			return E_FAIL;
		}
		if (FAILED(m_renderer->Initialize(m_Window->GetWindow())))
		{
			MessageBox(
				nullptr,
				L"Call to Renderer Initialize failed!",
				L"D3D12_DXR",
				NULL
			);
			return E_FAIL;
		}
		return S_OK;
	}

	INT Game::Run()
	{
		MSG msg = { 0 };

		// Main message loop
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			m_renderer->Update();
			m_renderer->Render();
		}

		return static_cast<INT>(msg.wParam);
	}
}