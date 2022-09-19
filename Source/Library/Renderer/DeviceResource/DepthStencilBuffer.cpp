#include "DepthStencilBuffer.h"

namespace library
{
	DepthStencilBuffer::DepthStencilBuffer(DXGI_FORMAT depthBufferFormat)
		: m_depthStencil(nullptr)
		, m_dsvDescriptorHeap(nullptr)
		, m_depthBufferFormat(depthBufferFormat)
	{
	}

	HRESULT DepthStencilBuffer::Init(Viewport viewport, ComPtr<ID3D12Device> device)
	{
		HRESULT hr = S_OK;

		if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN)
		{
			// create descriptor heap
			D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc =
			{
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
				.NumDescriptors = 1
			};
			hr = device->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(&m_dsvDescriptorHeap));
			if (FAILED(hr))
			{
				return hr;
			}

			// create resource
			UINT backBufferWidth = max(static_cast<int>(viewport.right - viewport.left), 1);
			UINT backBufferHeight = max(static_cast<int>(viewport.bottom - viewport.top), 1);

			D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			
			D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(m_depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1);
			depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			D3D12_CLEAR_VALUE optimizedClearValue =
			{
				.Format = m_depthBufferFormat,
				.DepthStencil = 
				{
					.Depth = 1.0f,
					.Stencil = 0
				},
			};

			hr = device->CreateCommittedResource(
				&heapProperty,
				D3D12_HEAP_FLAG_NONE,
				&depthStencilDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&optimizedClearValue,
				IID_PPV_ARGS(&m_depthStencil)
			);
			if (FAILED(hr))
			{
				return hr;
			}

			m_depthStencil->SetName(L"Depth Stencil");

			// create depthStencilView
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc =
			{
				.Format = m_depthBufferFormat,
				.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D
			};
			
			device->CreateDepthStencilView(m_depthStencil.Get(), &dsvDesc, m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			if (FAILED(hr))
			{
				return hr;
			}
		}

		return S_OK;
	}
}