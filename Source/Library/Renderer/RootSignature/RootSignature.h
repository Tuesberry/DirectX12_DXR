#pragma once

#include "Common.h"

namespace library 
{
	namespace GlobalRootSignatureParams {
		enum Value {
			OutputViewSlot = 0,
			AccelerationStructureSlot,
			Count
		};
	}

	

	enum class RootSignatureType
	{
		Global,
		Local
	};

	class RootSignature
	{
	public:
		RootSignature(RootSignatureType type);

		RootSignature() = delete;
		RootSignature(const RootSignature& other) = delete;
		RootSignature(RootSignature&& other) = delete;
		RootSignature& operator=(const RootSignature& other) = delete;
		RootSignature& operator=(RootSignature&& other) = delete;
		~RootSignature() = default;

		virtual HRESULT Init(ComPtr<ID3D12Device> device) = 0;

		ComPtr<ID3D12RootSignature> GetRootSignature() const { return m_rootSignature;  }
		
	protected:
		HRESULT CreateRootSignature(CD3DX12_ROOT_PARAMETER rootParams[], UINT rootParamsSize, ComPtr<ID3D12Device> device);

	private:
		ComPtr<ID3D12RootSignature> m_rootSignature;
		RootSignatureType m_rootSignatureType;
	};
}
