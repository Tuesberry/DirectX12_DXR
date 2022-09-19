#include "AccelerationStructure.h"

namespace library
{
	AccelerationStructure::AccelerationStructure()
		: m_accelerationStructure(nullptr)
		, m_bottomLevelAccelerationStructure(nullptr)
		, m_topLevelAccelerationStructure(nullptr)
	{
	}

	HRESULT AccelerationStructure::Build(
		ComPtr<ID3D12Device> device,
		ComPtr<ID3D12GraphicsCommandList> commandList,
		ComPtr<ID3D12CommandQueue> commandQueue,
		ComPtr<ID3D12CommandAllocator> commandAllocator,
		shared_ptr<Renderable> renderable
	)
	{
		// index, vertex buffer
		shared_ptr<D3DBuffer> indexBuffer = renderable->GetIndexBuffer();
		shared_ptr<D3DBuffer> vertexBuffer = renderable->GetVertexBuffer();

		// command list reset
		commandList->Reset(commandAllocator.Get(), nullptr);

		// Acceleration structure
		// 1. Geometry Á¤ÀÇ
		D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {
			.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES,
			.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE,	// mark the geometry as opaque
			.Triangles = {
				.Transform3x4 = 0,	// address of a 3X4 affine trasform matrix
				.IndexFormat = DXGI_FORMAT_R16_UINT,
				.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT,
				.IndexCount = static_cast<UINT>(indexBuffer->resource->GetDesc().Width) / sizeof(Index),
				.VertexCount = static_cast<UINT>(vertexBuffer->resource->GetDesc().Width) / sizeof(Vertex),
				.IndexBuffer = indexBuffer->resource->GetGPUVirtualAddress(),
				.VertexBuffer = {
					.StartAddress = vertexBuffer->resource->GetGPUVirtualAddress(),
					.StrideInBytes = sizeof(Vertex),
				},
			},
		};

		// 2. Get required sizes for an acceleration structure
		// defines the inputs for a raytracing acceleration structure build operation
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& bottomLevelInputs = bottomLevelBuildDesc.Inputs;
		bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		bottomLevelInputs.Flags = buildFlags;
		bottomLevelInputs.NumDescs = 1;
		bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		bottomLevelInputs.pGeometryDescs = &geometryDesc;

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& topLevelInputs = topLevelBuildDesc.Inputs;
		topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		topLevelInputs.Flags = buildFlags;
		topLevelInputs.NumDescs = 1;
		topLevelInputs.pGeometryDescs = nullptr;
		topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

		return S_OK;
	}
}