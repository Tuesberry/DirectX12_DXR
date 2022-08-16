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
		void WaitForPreviousFrame();
		void Update();
		void Render();

	private:
		// pipeline objects
		ComPtr<ID3D12Device> m_device;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D12Resource> m_backBuffers[g_numFrameBuffers];
		ComPtr<ID3D12GraphicsCommandList> m_commandList;
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
		ComPtr<ID3D12Fence> m_fence;
		ComPtr<ID3D12RootSignature> m_rootSignature;
		ComPtr<ID3D12PipelineState> m_pipelineState;
		
		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;
		
		// resource
		ComPtr<ID3D12Resource> m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

		D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle[g_numFrameBuffers];
		UINT m_rtvDescriptorSize;
		UINT m_currentBackBufferIndex;
		uint64_t m_fenceValue;
		HANDLE m_fenceEvent;
	};
}