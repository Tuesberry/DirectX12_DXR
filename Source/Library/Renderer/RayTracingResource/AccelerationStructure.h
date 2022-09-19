#pragma once

#include "Common.h"
#include "Renderer/Renderable.h"

namespace library
{
	class AccelerationStructure
	{
	public:
		AccelerationStructure();
		AccelerationStructure(const AccelerationStructure& other) = delete;
		AccelerationStructure(AccelerationStructure&& other) = delete;
		AccelerationStructure& operator=(const AccelerationStructure& other) = delete;
		AccelerationStructure& operator=(AccelerationStructure&& other) = delete;
		~AccelerationStructure() = default;

		HRESULT Build(
			ComPtr<ID3D12Device> device,
			ComPtr<ID3D12GraphicsCommandList> commandList,
			ComPtr<ID3D12CommandQueue> commandQueue,
			ComPtr<ID3D12CommandAllocator> commandAllocator,
			shared_ptr<Renderable> renderable
		);

	private:
		ComPtr<ID3D12Resource> m_accelerationStructure;
		ComPtr<ID3D12Resource> m_bottomLevelAccelerationStructure;
		ComPtr<ID3D12Resource> m_topLevelAccelerationStructure;
	};
}