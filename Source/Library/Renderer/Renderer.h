#pragma once

#include "Common.h"

namespace library
{
	class Renderer final
	{
	public:
		Renderer();
		Renderer(const Renderer& other) = delete;
		Renderer(Renderer&& other) = delete;
		Renderer& operator=(const Renderer& other) = delete;
		Renderer& operator=(Renderer&& other) = delete;
		~Renderer() = default;

		HRESULT Initialize(_In_ HWND hWnd);
		void Update();
		void Render();

	private:
		ComPtr<ID3D12Device> m_Device;
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<IDXGISwapChain> m_SwapChain;
		ComPtr<ID3D12Resource> m_BackBuffers[g_NumFrameBuffers];
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
		ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap;
		ComPtr<ID3D12Fence> m_Fence;

		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;

		D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandle[g_NumFrameBuffers];
		UINT m_RTVDescriptorSize;
		UINT m_CurrentBackBufferIndex;
		uint64_t m_FenceValue;
		HANDLE m_FenceEvent;
	};
}