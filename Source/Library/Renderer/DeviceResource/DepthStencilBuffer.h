#pragma once

#include "Common.h"

namespace library
{
	class DepthStencilBuffer final
	{
	public:
		DepthStencilBuffer(
			DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT
		);
		DepthStencilBuffer(const DepthStencilBuffer& other) = delete;
		DepthStencilBuffer(DepthStencilBuffer&& other) = delete;
		DepthStencilBuffer& operator=(const DepthStencilBuffer& other) = delete;
		DepthStencilBuffer& operator=(DepthStencilBuffer&& other) = delete;
		~DepthStencilBuffer() = default;

		HRESULT Init(Viewport viewport, ComPtr<ID3D12Device> device);

	private:
		// depth Stencil View
		ComPtr<ID3D12Resource> m_depthStencil;
		ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;

		// Direct3D properties.
		DXGI_FORMAT m_depthBufferFormat;
	};
}