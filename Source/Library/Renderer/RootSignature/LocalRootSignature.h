#pragma once

#include "Common.h"
#include "RootSignature.h"

namespace library
{
	class LocalRootSignature : public RootSignature
	{
	public:
		enum LocalRootSignatureParams
		{
			CubeConstantSlot = 0,
			Count
		};

		LocalRootSignature();
		LocalRootSignature(const LocalRootSignature& other) = delete;
		LocalRootSignature(LocalRootSignature&& other) = delete;
		LocalRootSignature& operator=(const LocalRootSignature& other) = delete;
		LocalRootSignature& operator=(LocalRootSignature&& other) = delete;
		~LocalRootSignature() = default;

		HRESULT Init(ComPtr<ID3D12Device> device) final;
	};
}