#include "Cube.h"

namespace library
{
	Cube::Cube()
		: Renderable()
	{
	}

	HRESULT Cube::Init(ComPtr<ID3D12Device> device, shared_ptr<DescriptorHeap> descriptorHeap)
	{
		HRESULT hr = S_OK;

		hr = CreateBuffer(VERTICES, sizeof(VERTICES), INDICES, sizeof(INDICES), device);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = CreateView(ARRAYSIZE(VERTICES), sizeof(VERTICES[0]), sizeof(INDICES) / 4, 0, device, descriptorHeap);
		if (FAILED(hr))
		{
			return hr;
		}

		return S_OK;
	}
}