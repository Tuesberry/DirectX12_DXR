#include "Renderable.h"

namespace library
{
	Renderable::Renderable()
		: m_indexBuffer(nullptr)
		, m_vertexBuffer(nullptr)
	{
	}

	HRESULT Renderable::CreateBuffer(const void* vertexData, UINT64 vertexDataSize, const void* indexData, UINT64 indexDataSize, ComPtr<ID3D12Device> device)
	{
		HRESULT hr = S_OK;
		
		// heap Properties
		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

		// index Buffer
		D3D12_RESOURCE_DESC indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexDataSize);
		hr = device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &indexBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_indexBuffer->resource.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}

		void* pIndexMappedData;
		m_vertexBuffer->resource->Map(0, nullptr, &pIndexMappedData);
		memcpy(pIndexMappedData, indexData, indexDataSize);
		m_vertexBuffer->resource->Unmap(0, nullptr);

		// vertex Buffer
		D3D12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexDataSize);
		hr = device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &vertexBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_vertexBuffer->resource.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}

		// map 함수를 통해 Upload 자원에 데이터를 넣어 둔다.
		void* pVertexMappedData;
		m_vertexBuffer->resource->Map(0, nullptr, &pVertexMappedData);
		memcpy(pVertexMappedData, vertexData, vertexDataSize);
		m_vertexBuffer->resource->Unmap(0, nullptr);

		return S_OK;
	}

	HRESULT Renderable::CreateView(UINT numVertexElement, UINT vertexElementSize, UINT numIndexElement, UINT indexElementSize, ComPtr<ID3D12Device> device, shared_ptr<DescriptorHeap> descriptorHeap)
	{
		UINT descriptorIndexIB = CreateBufferSRV(m_indexBuffer, numIndexElement, indexElementSize, device, descriptorHeap);
		UINT descriptorIndexVB = CreateBufferSRV(m_vertexBuffer, numVertexElement, vertexElementSize, device, descriptorHeap);

		// Vertex buffer is passed to the shader along with index buffer as a descriptor table.
		// Vertex buffer descriptor must follow index buffer descriptor in the descriptor heap.
		if (descriptorIndexVB != descriptorIndexIB + 1)
		{
			return E_FAIL;
		}

		return S_OK;
	}

	UINT Renderable::CreateBufferSRV(shared_ptr<D3DBuffer> buffer, UINT numElements, UINT elementSize, ComPtr<ID3D12Device> device, shared_ptr<DescriptorHeap> descriptorHeap)
	{
		// shader resource view(SRV)
		// 셰이더가 자원을 읽을 수 있도록 함
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc =
		{
			.ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
			.Buffer =
			{
				.NumElements = numElements,
			}
		};
		if (elementSize == 0)
		{
			srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
			srvDesc.Buffer.StructureByteStride = 0;
		}
		else
		{
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			srvDesc.Buffer.StructureByteStride = elementSize;
		}

		// allocate descriptor
		UINT descriptorIndex = descriptorHeap->AllocateDescriptor(&buffer->cpuDescriptorHandle);
		device->CreateShaderResourceView(buffer->resource.Get(), &srvDesc, buffer->cpuDescriptorHandle);
		buffer->gpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(descriptorHeap->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(), descriptorIndex, descriptorHeap->GetDescriptorSize());
		return descriptorIndex;
	}
}