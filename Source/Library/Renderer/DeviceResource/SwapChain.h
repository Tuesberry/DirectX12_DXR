#pragma once

#include "Common.h"

namespace library
{
	class SwapChain final
	{
	public:
		SwapChain(
			DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM
		);
		SwapChain(const SwapChain& other) = delete;
		SwapChain(SwapChain&& other) = delete;
		SwapChain& operator=(const SwapChain& other) = delete;
		SwapChain& operator=(SwapChain&& other) = delete;
		~SwapChain() = default;

		HRESULT Init(HWND hWnd, Viewport viewport, ComPtr<IDXGIFactory> dxgiFactory, ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Device> device);
		HRESULT Update(Viewport viewport);

		ComPtr<IDXGISwapChain3> GetSwapChain() { return m_swapChain; }
		ComPtr<ID3D12Resource> GetRenderTargets(UINT index) { return m_renderTargets[index]; }
		DXGI_FORMAT GetBackBufferFormat() { return m_backBufferFormat; }

	private:
		HRESULT CreateDescriptorHeaps(ComPtr<ID3D12Device> device);
		HRESULT CreateSwapChain(HWND hWnd, Viewport viewport, ComPtr<IDXGIFactory> dxgiFactory, ComPtr<ID3D12CommandQueue> commandQueue);
		HRESULT CreateRTV(ComPtr<ID3D12Device> device);

	private:
		// swap chain objects
		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D12Resource> m_renderTargets[MAX_BACK_BUFFER_COUNT];

		// Direct3D properties.
		DXGI_FORMAT m_backBufferFormat;
		UINT m_backBufferCount;

		ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
		UINT m_rtvDescriptorSize;
	};
}