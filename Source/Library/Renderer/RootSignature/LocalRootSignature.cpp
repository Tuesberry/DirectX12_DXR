#include "LocalRootSignature.h"

namespace library
{
	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   LocalRootSignature::LocalRootSignature
	  Summary:  Constructor
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	LocalRootSignature::LocalRootSignature()
		:RootSignature(RootSignatureType::Local)
	{
	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   LocalRootSignature::Init
	  Summary:  Create root signatures for ths shaders
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	HRESULT LocalRootSignature::Init(ComPtr<ID3D12Device> device)
	{
		// local Root Signature
		// This is a root signature that enables a shader 
		// to have unique arguments that come from shader tables.
		
		HRESULT hr = S_OK;

		// Root parameter
		CD3DX12_ROOT_PARAMETER rootParameters[LocalRootSignatureParams::Count];
		rootParameters[LocalRootSignatureParams::CubeConstantSlot].InitAsConstants((sizeof(CubeConstantBuffer)-1)/sizeof(UINT32)+1, 0, 0);
		
		// Create Root Signature
		hr = CreateRootSignature(rootParameters, ARRAYSIZE(rootParameters), device);
		if (FAILED(hr))
		{
			return hr;
		}

		return hr;
	}
}