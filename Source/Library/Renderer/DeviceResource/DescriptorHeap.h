#pragma once

#include "Common.h"

namespace library
{
	class DescriptorHeap
	{
	public:
		DescriptorHeap(UINT numDescriptors = 1);

		DescriptorHeap() = delete;
		DescriptorHeap(const DescriptorHeap& other) = delete;
		DescriptorHeap(DescriptorHeap&& other) = delete;
		DescriptorHeap& operator=(const DescriptorHeap& other) = delete;
		DescriptorHeap& operator=(DescriptorHeap&& other) = delete;
		~DescriptorHeap() = default;

		HRESULT Init(ComPtr<ID3D12Device> device);
		UINT AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse = UINT_MAX);

		ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const { return m_descriptorHeap; }
		UINT GetDescriptorSize() { return m_descriptorSize; }

	private:
		ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
		UINT m_descriptorsAllocated;
		UINT m_descriptorSize;
		UINT m_numDescriptors;
	};
}