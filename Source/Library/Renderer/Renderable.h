#pragma once

#include "Common.h"
#include "Renderer/DeviceResource/DescriptorHeap.h"

namespace library
{
	struct D3DBuffer
	{
		ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;
	};

	class Renderable
	{
	public:
		Renderable();
		Renderable(const Renderable& other) = delete;
		Renderable(Renderable&& other) = delete;
		Renderable& operator=(const Renderable& other) = delete;
		Renderable& operator=(Renderable&& other) = delete;
		~Renderable() = default;

		virtual HRESULT Init(ComPtr<ID3D12Device> device, shared_ptr<DescriptorHeap> descriptorHeap) = 0;

		HRESULT CreateBuffer(
			const void* vertexData, 
			UINT64 vertexDataSize, 
			const void* indexData, 
			UINT64 indexDataSize, 
			ComPtr<ID3D12Device> device
		);
		
		HRESULT CreateView(
			UINT numVertexElement, 
			UINT vertexElementSize, 
			UINT numIndexElement, 
			UINT indexElementSize, 
			ComPtr<ID3D12Device> device, 
			shared_ptr<DescriptorHeap> descriptorHeap
		);

		shared_ptr<D3DBuffer> GetIndexBuffer() const { return m_indexBuffer; }
		shared_ptr<D3DBuffer> GetVertexBuffer() const { return m_vertexBuffer; }

	private:
		UINT CreateBufferSRV(
			shared_ptr<D3DBuffer> buffer, 
			UINT numElements, 
			UINT elementSize, 
			ComPtr<ID3D12Device> device, 
			shared_ptr<DescriptorHeap> descriptorHeap
		);

		shared_ptr<D3DBuffer> m_indexBuffer;
		shared_ptr<D3DBuffer> m_vertexBuffer;
	};
}