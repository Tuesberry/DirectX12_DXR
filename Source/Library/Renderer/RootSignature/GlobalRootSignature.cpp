#include "GlobalRootSignature.h"

namespace library
{
	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   GlobalRootSignature::GlobalRootSignature
	  Summary:  Constructor
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	GlobalRootSignature::GlobalRootSignature()
		:RootSignature(RootSignatureType::Global)
	{
	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   GlobalRootSignature::Init
	  Summary:  Create root signatures for ths shaders
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	HRESULT GlobalRootSignature::Init(ComPtr<ID3D12Device> device)
	{
		// local Root Signature
		// This is a root signature that enables a shader 
		// to have unique arguments that come from shader tables.

		HRESULT hr = S_OK;

		CD3DX12_DESCRIPTOR_RANGE ranges[2]; // Perfomance TIP: Order from most frequent to least frequent.
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);  // 1 output texture
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);  // 2 static index and vertex buffers.

		// Root parameter
		CD3DX12_ROOT_PARAMETER rootParameters[GlobalRootSignatureParams::Count];
		rootParameters[GlobalRootSignatureParams::OutputViewSlot].InitAsDescriptorTable(1, &ranges[0]);
		rootParameters[GlobalRootSignatureParams::AccelerationStructureSlot].InitAsShaderResourceView(0);
		rootParameters[GlobalRootSignatureParams::SceneConstantSlot].InitAsConstantBufferView(0);
		rootParameters[GlobalRootSignatureParams::VertexBuffersSlot].InitAsDescriptorTable(1, &ranges[1]);

		// Create Root Signature
		hr = CreateRootSignature(rootParameters, ARRAYSIZE(rootParameters), device);
		if (FAILED(hr))
		{
			return hr;
		}

		return hr;
	}
}