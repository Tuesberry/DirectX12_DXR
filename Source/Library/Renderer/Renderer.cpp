#include "Renderer.h"

namespace library
{
	Renderer::Renderer()
		:m_Device(nullptr)
		,m_CommandQueue(nullptr)
		,m_SwapChain(nullptr)
		,m_BackBuffers{}
		,m_CommandList(nullptr)
		,m_CommandAllocator{}
		,m_RTVDescriptorHeap(nullptr)
		,m_Fence(nullptr)
		,m_Viewport{0, 0, 0.0f, 0.0f, 0.0f, 1.0f}
		,m_ScissorRect{}
		,m_RTVHandle{}
		,m_RTVDescriptorSize(0)
		,m_CurrentBackBufferIndex(0)
		,m_FenceValue(0)
		,m_FenceEvent(INVALID_HANDLE_VALUE)
	{
	}

	HRESULT Renderer::Initialize(_In_ HWND hWnd)
	{
		HRESULT hr = S_OK;

		RECT rc;
		GetClientRect(hWnd, &rc);
		UINT uWidth = static_cast<UINT>(rc.right - rc.left);
		UINT uHeight = static_cast<UINT>(rc.bottom - rc.top);

		// set viewport
		m_Viewport.Width = static_cast<float>(uWidth);
		m_Viewport.Height = static_cast<float>(uHeight);

		// set rect
		m_ScissorRect = CD3DX12_RECT(0, 0, uWidth, uHeight);

		// 1. create the device
		// create dxgiFactory
		ComPtr<IDXGIFactory> dxgiFactory(nullptr);
		hr = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}
		// createDevice
		hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_Device.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}

		// 2. Create a direct command queue
		D3D12_COMMAND_QUEUE_DESC queueDesc = {
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
		};
		hr = m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}

		// 3. create the swap chain
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {
			.BufferDesc = {
				.Width = uWidth,
				.Height = uHeight,
				.RefreshRate = {
					.Numerator = 60,
					.Denominator = 1
				},
				.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
				.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
				.Scaling = DXGI_MODE_SCALING_UNSPECIFIED
			},
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = g_NumFrameBuffers,
			.OutputWindow = hWnd,
			.Windowed = true,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		};

		hr = dxgiFactory->CreateSwapChain(m_CommandQueue.Get(), &swapChainDesc, m_SwapChain.GetAddressOf());
		if (FAILED(hr))
		{
			return hr;
		}

		// 4. Create the Descriptor Heap
		// rtv descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = g_NumFrameBuffers,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0
		};
		hr = m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_RTVDescriptorHeap.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}
		// get the size of a descriptor in this heap
		m_RTVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		// get a handle to the first descriptor in the descriptor heap
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		// create a RTV for each buffer
		for (UINT i = 0; i < g_NumFrameBuffers; ++i)
		{
			hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_BackBuffers[i].GetAddressOf()));
			if (FAILED(hr))
			{
				return hr;
			}
			m_RTVHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * m_RTVDescriptorSize);
			m_Device->CreateRenderTargetView(m_BackBuffers[i].Get(), nullptr, m_RTVHandle[i]);
		}

		// 5. Create the Command Allocator
		hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}

		// 6. Create a Command list
		hr = m_Device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_CommandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(m_CommandList.GetAddressOf())
		);
		if (FAILED(hr))
		{
			return hr;
		}
		// close command list
		m_CommandList->Close();

		// 7. Create the fences
		hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}
		m_FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		return S_OK;
	}

	void Renderer::Update()
	{

	}

	void Renderer::Render()
	{
		m_CommandAllocator->Reset();
		m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);

		// Clear the render target
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_BackBuffers[m_CurrentBackBufferIndex].Get(), 
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_CommandList->ResourceBarrier(1, &barrier);
		
		m_CommandList->RSSetViewports(1, &m_Viewport);
		m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		CD3DX12_CPU_DESCRIPTOR_HANDLE backBufferView(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex, m_RTVDescriptorSize);
		m_CommandList->ClearRenderTargetView(backBufferView, Colors::MidnightBlue, 0, nullptr);
		m_CommandList->OMSetRenderTargets(1, &backBufferView, FALSE, nullptr);

		// present
		CD3DX12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
			m_BackBuffers[m_CurrentBackBufferIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_CommandList->ResourceBarrier(1, &barrier2);
		m_CommandList->Close();

		ID3D12CommandList* const commandListArr[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(commandListArr), commandListArr);
		
		m_SwapChain->Present(0, 0);
		
		m_FenceValue++;
		m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue);
		if (m_Fence->GetCompletedValue() < m_FenceValue)
		{
			m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
			WaitForSingleObject(m_FenceEvent, INFINITE);
		}

		m_CurrentBackBufferIndex = (m_CurrentBackBufferIndex + 1) % g_NumFrameBuffers;
	}
}