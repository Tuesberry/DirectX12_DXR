#include "ConstantBuffer.h"

namespace library
{
	ConstantBuffer::ConstantBuffer()
		: m_elementCount(0)
		, m_elementSize(0)
		, m_mappedConstantData(nullptr)
		, m_resource(nullptr)
	{
	}

	HRESULT ConstantBuffer::Allocate(ComPtr<ID3D12Device> device, UINT bufferSize, LPCWSTR resourceName = nullptr)
	{
		HRESULT hr = S_OK;

		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

		// create resource
		hr = device->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_resource)
		);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = m_resource->SetName(resourceName);
		if (FAILED(hr))
		{
			return hr;
		}

		// map
		CD3DX12_RANGE readRange(0, 0);
		hr = m_resource->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedConstantData));
		if (FAILED(hr))
		{
			return hr;
		}

		return S_OK;
	}

	HRESULT ConstantBuffer::Create(ComPtr<ID3D12Device> device, UINT elementCount, UINT size, LPCWSTR resourceName = nullptr)
	{
		HRESULT hr = S_OK;

		// constant buffer는 256바이트의 배수여야 함
		m_elementCount = elementCount;
		m_elementSize = (size + 255) & ~255;

		// Allocate
		hr = Allocate(device, m_elementSize * m_elementCount, resourceName);
		if (FAILED(hr))
		{
			return hr;
		}

		return S_OK;
	}

}