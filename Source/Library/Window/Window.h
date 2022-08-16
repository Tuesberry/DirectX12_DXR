/*+===================================================================
  File:      WINDOW.H
  Summary:   Window header files
  Classes: Window
  Functions: InitWindow, InitDevice
===================================================================+*/
#pragma once

#include "Common.h"

namespace library 
{
	class Window
	{
	protected:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	public:
		Window();
		Window(const Window& rhs) = delete;
		Window(Window&& rhs) = delete;
		Window& operator=(const Window& rhs) = delete;
		Window& operator=(Window&& rhs) = delete;
		~Window() = default;

		HRESULT Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow, _In_ PCWSTR pszWindowName);
		LRESULT HandleMessage(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
		
		PCWSTR GetWindowClassName() const;
		HWND GetWindow() const;

	protected:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		LPCWSTR m_pszWindowName;
	};
}