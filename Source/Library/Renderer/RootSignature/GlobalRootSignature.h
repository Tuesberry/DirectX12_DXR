#pragma once

#include "Common.h"
#include "RootSignature.h"

namespace library
{
	class GlobalRootSignature : public RootSignature
	{
	public:
		enum GlobalRootSignatureParams
		{
			OutputViewSlot = 0,
			AccelerationStructureSlot,
			SceneConstantSlot,
			VertexBuffersSlot,
			Count
		};

		GlobalRootSignature();
		GlobalRootSignature(const GlobalRootSignature& other) = delete;
		GlobalRootSignature(GlobalRootSignature&& other) = delete;
		GlobalRootSignature& operator=(const GlobalRootSignature& other) = delete;
		GlobalRootSignature& operator=(GlobalRootSignature&& other) = delete;
		~GlobalRootSignature() = default;

		HRESULT Init(ComPtr<ID3D12Device> device) final;
	};
}