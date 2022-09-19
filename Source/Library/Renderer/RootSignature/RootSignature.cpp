#include "RootSignature.h"

namespace library 
{
	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   RootSignature::RootSignature
	  Summary:  Constructor
	  Modifies: [m_rootSignature, m_rootSignatureType].
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	RootSignature::RootSignature(RootSignatureType type)
		: m_rootSignature(nullptr)
		, m_rootSignatureType(type)
	{
	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   RootSignature::CreateRootSignature
	  Summary:  create root signature desc and serialize and create root signature
	  Args:		CD3DX12_ROOT_PARAMETER rootParams[], UINT rootParamsSize, ComPtr<ID3D12Device> device
	  Modifies: [m_rootSignature].
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	HRESULT RootSignature::CreateRootSignature(CD3DX12_ROOT_PARAMETER rootParams[], UINT rootParamsSize, ComPtr<ID3D12Device> device)
	{
		HRESULT hr = S_OK;

		// Root signature desc
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(rootParamsSize, rootParams);
		
		// if Local Root Signature
		if (m_rootSignatureType == RootSignatureType::Local)
		{
			rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
		}

		// serialize and create root signature
		ComPtr<ID3DBlob> blob = nullptr;
		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr);
		if (FAILED(hr))
		{
			return hr;
		}
		hr = device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
		if (FAILED(hr))
		{
			return hr;
		}

		return S_OK;
	}
}