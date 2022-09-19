#include "SwapChain.h"

namespace library
{
	SwapChain::SwapChain(DXGI_FORMAT backBufferFormat)
		: m_swapChain(nullptr)
		, m_renderTargets{ nullptr }
		, m_backBufferFormat(backBufferFormat)
		, m_backBufferCount(MAX_BACK_BUFFER_COUNT)
		, m_rtvDescriptorHeap(nullptr)
		, m_rtvDescriptorSize()
	{
	}

	HRESULT SwapChain::Init(HWND hWnd, Viewport viewport, ComPtr<IDXGIFactory> dxgiFactory, ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Device> device)
	{
		HRESULT hr = S_OK;

		// create descriptor heaps for render target views and depth stencil views
		hr = CreateDescriptorHeaps(device);
		if (FAILED(hr))
		{
			return hr;
		}
		// create swap chain
		hr = CreateSwapChain(hWnd, viewport, dxgiFactory, commandQueue);
		if (FAILED(hr))
		{
			return hr;
		}
		// create render target view
		hr = CreateRTV(device);
		if (FAILED(hr))
		{
			return hr;
		}

		return S_OK;
	}

	HRESULT SwapChain::Update(Viewport viewport)
	{
		HRESULT hr S_OK;

		UINT backBufferWidth = max(static_cast<int>(viewport.right - viewport.left), 1);
		UINT backBufferHeight = max(static_cast<int>(viewport.bottom - viewport.top), 1);

		if (m_swapChain)
		{
			// if swap chain exist
			hr = m_swapChain->ResizeBuffers(m_backBufferCount, backBufferWidth, backBufferHeight, m_backBufferFormat, 0);
			if (FAILED(hr))
			{
				return hr;
			}
		}

		return S_OK;
	}

	HRESULT SwapChain::CreateDescriptorHeaps(ComPtr<ID3D12Device> device)
	{
		HRESULT hr = S_OK;

		// create descriptor heaps for render target view
		D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc =
		{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = m_backBufferCount,
		};

		hr = device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&m_rtvDescriptorHeap));
		if (FAILED(hr))
		{
			return hr;
		}

		// get descriptor size
		m_rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		return S_OK;
	}

	HRESULT SwapChain::CreateSwapChain(HWND hWnd, Viewport viewport, ComPtr<IDXGIFactory> dxgiFactory, ComPtr<ID3D12CommandQueue> commandQueue)
	{
		HRESULT hr = S_OK;

		UINT backBufferWidth = max(static_cast<int>(viewport.right - viewport.left), 1);
		UINT backBufferHeight = max(static_cast<int>(viewport.bottom - viewport.top), 1);

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {
			.BufferDesc = {
				.Width = backBufferWidth,
				.Height = backBufferHeight,
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
			.BufferCount = m_backBufferCount,
			.OutputWindow = hWnd,
			.Windowed = true,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		};

		ComPtr<IDXGISwapChain> swapChain = nullptr;
		hr = dxgiFactory->CreateSwapChain(commandQueue.Get(), &swapChainDesc, swapChain.GetAddressOf());
		if (FAILED(hr))
		{
			return hr;
		}

		hr = swapChain.As(&m_swapChain);
		if (FAILED(hr))
		{
			return hr;
		}

		return S_OK;
	}

	HRESULT SwapChain::CreateRTV(ComPtr<ID3D12Device> device)
	{
		HRESULT hr = S_OK;

		for (UINT n = 0; n < m_backBufferCount; n++)
		{
			hr = m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n]));
			if (FAILED(hr))
			{
				return hr;
			}

			wchar_t name[25] = {};
			swprintf_s(name, L"Render target %u", n);
			m_renderTargets[n]->SetName(name);

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc =
			{
				.Format = m_backBufferFormat,
				.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
			};

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), n, m_rtvDescriptorSize);
			device->CreateRenderTargetView(m_renderTargets[n].Get(), &rtvDesc, rtvDescriptor);
		}

		return S_OK;
	}
}