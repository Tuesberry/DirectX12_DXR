#include "DescriptorHeap.h"

namespace library
{
	DescriptorHeap::DescriptorHeap(UINT numDescriptors)
		: m_descriptorHeap(nullptr)
		, m_descriptorsAllocated(0)
		, m_descriptorSize(0)
		, m_numDescriptors(numDescriptors)
	{
	}

	HRESULT DescriptorHeap::Init(ComPtr<ID3D12Device> device)
	{
		HRESULT hr = S_OK;

		// Allocate a descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			.NumDescriptors = m_numDescriptors,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
			.NodeMask = 0
		};

		// create descriptor heap
		hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));
		if (FAILED(hr))
		{
			return hr;
		}

		// get descriptor size
		m_descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
		return S_OK;
	}

	UINT DescriptorHeap::AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHeapCpuBase = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		if (descriptorIndexToUse >= m_descriptorHeap->GetDesc().NumDescriptors)
		{
			descriptorIndexToUse = m_descriptorsAllocated++;
		}
		*cpuDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeapCpuBase, descriptorIndexToUse, m_descriptorSize);
		return descriptorIndexToUse;
	}

}