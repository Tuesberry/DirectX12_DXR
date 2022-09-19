#include "Renderer.h"
#include "CompiledShaders/Raytracing.hlsl.h"

namespace library
{
	const wchar_t* Renderer::m_hitGroupName = L"MyHitGroup";
	const wchar_t* Renderer::m_raygenShaderName = L"MyRaygenShader";
	const wchar_t* Renderer::m_closestHitShaderName = L"MyClosestHitShader";
	const wchar_t* Renderer::m_missShaderName = L"MyMissShader";

	Renderer::Renderer(UINT width , UINT height , std::wstring name)
		: m_window(nullptr)
		, m_viewport()
		, m_title(name)
		, m_device(make_shared<Device>())
		, m_commandQueue(make_shared<CommandQueue>())
		, m_depthStencilBuffer(make_shared<DepthStencilBuffer>(DXGI_FORMAT_UNKNOWN))
		, m_swapChain(make_shared<SwapChain>(DXGI_FORMAT_R8G8B8A8_UNORM))
		, m_raytracingGlobalRootSignature(make_shared<GlobalRootSignature>())
		, m_raytracingLocalRootSignature(make_shared<LocalRootSignature>())
		, m_dxrStateObject(nullptr)
		, m_descriptorHeap(make_shared<DescriptorHeap>())
		, m_sceneCB()
		, m_cubeCB()
		, m_cube(make_shared<Cube>())
		, m_accelerationStructure(nullptr)
		, m_bottomLevelAccelerationStructure(nullptr)
		, m_topLevelAccelerationStructure(nullptr)
		, m_raytracingOutput(nullptr)
		, m_raytracingOutputResourceUAVGpuDescriptor()
		, m_raytracingOutputResourceUAVDescriptorHeapIndex(0)
		, m_missShaderTable(nullptr)
		, m_hitGroupShaderTable(nullptr)
		, m_rayGenShaderTable(nullptr)
	{
		m_viewport.left = m_viewport.top = 0;
		m_viewport.right = width;
		m_viewport.bottom = height;
	}

	HRESULT Renderer::Initialize(HWND hWnd)
	{
		HRESULT hr = S_OK;

		// init device Resources
		hr = CreateDeviceResources();
		if (FAILED(hr))
		{
			MessageBox(
				nullptr,
				L"Renderer | Call to CreateDeviceResources failed!",
				L"D3D12_DXR",
				NULL
			);
			return E_FAIL;
		}
		
		// init scene

		// ray tracing device resources
		hr = CreateRaytracingDeviceResources();
		if (FAILED(hr))
		{
			MessageBox(
				nullptr,
				L"Renderer | Call to CreateRaytracingDeviceResources failed!",
				L"D3D12_DXR",
				NULL
			);
			return E_FAIL;
		}

		return S_OK;
	}

	HRESULT Renderer::CreateDeviceResources()
	{
		HRESULT hr = S_OK;

		// initialize d3dDevice5
		hr = m_device->Init();
		if (FAILED(hr))
		{
			return hr;
		}
		// initialize command queue
		hr = m_commandQueue->Init(m_device->GetD3DDevice(), m_swapChain);
		if (FAILED(hr))
		{
			return hr;
		}
		// initialize Swap Chain
		hr = m_swapChain->Init(m_window, m_viewport, m_device->GetDXGIFactory(), m_commandQueue->GetCommandQueue(), m_device->GetD3DDevice());
		if (FAILED(hr))
		{
			return hr;
		}
		// reset back buffer index
		m_commandQueue->UpdateBackBufferIndex();
		// initialize depth/Stencil buffer
		hr = m_depthStencilBuffer->Init(m_viewport, m_device->GetD3DDevice());
		if (FAILED(hr))
		{
			return hr;
		}
		return S_OK;
	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Renderer::CreateRaytracingDeviceResources
	  Summary:  Create resources that depend on the device
				initialize raytracing pipeline
	  Modifies: [].
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	HRESULT Renderer::CreateRaytracingDeviceResources()
	{
		HRESULT hr = S_OK;

		// create root signatures for the shaders
		hr = m_raytracingGlobalRootSignature->Init(m_device->GetD3DDevice());
		if (FAILED(hr))
		{
			return hr;
		}
		hr = m_raytracingLocalRootSignature->Init(m_device->GetD3DDevice());
		if (FAILED(hr))
		{
			return hr;
		}

		// create a raytracing pipeline state object 
		// which defines the binding of shaders, state and resources to be used during raytracing.
		hr = CreateRaytracingPipelineStateObject();
		if (FAILED(hr))
		{
			return hr;
		}

		// create a heap for descriptors
		hr = m_descriptorHeap->Init(m_device->GetD3DDevice());
		if (FAILED(hr))
		{
			return hr;
		}

		// build geometry to be used
		hr = m_cube->Init(m_device->GetD3DDevice(), m_descriptorHeap);
		if (FAILED(hr))
		{
			return hr;
		}

		// Build raytracing acceleration structures from the generated geometry
		BuildAccelerationStructres();

		// create constant buffers for the geometry and the scene


		// build shader tables, which define shaders and their local root arguments
		BuildShaderTables();

		// create an output 2D texture to store the raytracing result to
		CreateRaytracingOutputResource();
	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Renderer::CreateRaytracingPipelineStateObject
	  Summary:  Create a raytracing pipeline state object (RTPSO)
				An RTPSO represents a full set of shaders reachable by a 
				DispatchRays() call with all configuration options resolved, 
				such as local signatures and other state.
	  Modifies: [].
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	HRESULT Renderer::CreateRaytracingPipelineStateObject()
	{
		// Create 7 subobjects that combine into a RTPSO:
		// Subobjects need to be associated with DXIL exports (i.e. shaders) either by way of default or explicit associations.
		// Default association applies to every exported shader entrypoint that doesn't have any of the same type of subobject associated with it.
		// This simple sample utilizes default shader association except for local root signature subobject
		// which has an explicit association specified purely for demonstration purposes.
		// 1 - DXIL library
		// 1 - Triangle hit group
		// 1 - Shader config
		// 2 - Local root signature and association
		// 1 - Global root signature
		// 1 - Pipeline config
		CD3DX12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

		// DXIL library
		// shader와 state object의 진입점을 포함, 셰이더는 subobject가 아니기 때문에 DXIL library subobjects를 통해 전달해야 함
		auto lib = raytracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
		// compile shader
		D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE((void*)g_pRaytracing, ARRAYSIZE(g_pRaytracing));
		lib->SetDXILLibrary(&libdxil);
		lib->DefineExport(m_raygenShaderName);
		lib->DefineExport(m_closestHitShaderName);
		lib->DefineExport(m_missShaderName);

		// Triangle hit group
		// hit group은 closest hit, any hit, intersection shaders를 지정함. ray가 geometry의 triangle/AABB와 교차하는 경우 shader가 실행됨
		auto hitGroup = raytracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
		hitGroup->SetClosestHitShaderImport(m_closestHitShaderName);
		hitGroup->SetHitGroupExport(m_hitGroupName);
		hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

		// Shader config
		// ray payload와 attribute structure의 최대 크기를 정의함
		auto shaderConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
		UINT payloadSize = sizeof(XMFLOAT4);    // float4 pixelColor
		UINT attributeSize = sizeof(XMFLOAT2);  // float2 barycentrics
		shaderConfig->Config(payloadSize, attributeSize);

		// Local root signature and shader association
		// hit group과 miss shader는 local root signature를 사용하지 않는다
		// Ray generation shader에서 local root signature가 사용됨
		auto localRootSignature = raytracingPipeline.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		localRootSignature->SetRootSignature(m_raytracingLocalRootSignature->GetRootSignature().Get());
		// shader association
		auto rootSignatureAssociation = raytracingPipeline.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
		rootSignatureAssociation->AddExport(m_hitGroupName);

		// global root signature
		// DispatchRays()를 호출하는 동안 모든 raytracing shader간 공유됨
		auto globalRootSignature = raytracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
		globalRootSignature->SetRootSignature(m_raytracingGlobalRootSignature->GetRootSignature().Get());

		// pipeline config
		// TraceRay() recursion depth의 max를 정의함
		auto pipelineConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
		UINT maxRecursionDepth = 1;
		pipelineConfig->Config(maxRecursionDepth);

		// Create the state object
		HRESULT hr = m_device->GetD3DDevice()->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&m_dxrStateObject));
		if (FAILED(hr))
		{
			return hr;
		}

		return S_OK;
	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Renderer::BuildAccelerationStructres
	  Summary:  Build acceleration structures needed for raytracing
	  Modifies: [].
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	HRESULT Renderer::BuildAccelerationStructres()
	{
		// TODO
	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Renderer::BuildShaderTables
	  Summary:  This encapsulates all shader records 
				shaders and the arguments for their local root signatures.
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	HRESULT Renderer::BuildShaderTables()
	{
		// TODO
	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Renderer::CreateRaytracingOutputResource
	  Summary:  Create 2D output texture for raytracing
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	void Renderer::CreateRaytracingOutputResource()
	{
		// TODO
	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Renderer::Update
      Summary:  Update the scene
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	void Renderer::Update()
	{

	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Renderer::Render
	  Summary:  Render the scene
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	void Renderer::Render()
	{
		m_commandQueue->Prepare();
		DoRaytracing();
		CopyRaytracingOutputToBackbuffer();
		m_commandQueue->Present(D3D12_RESOURCE_STATE_PRESENT);
	}

	void Renderer::DoRaytracing()
	{
		// TODO
	}

	void Renderer::CopyRaytracingOutputToBackbuffer()
	{
		// TODO
	}

	HRESULT Renderer::AllocateUploadBuffer(void* pData, UINT64 datasize, ComPtr<ID3D12Resource> pResource, LPCWSTR resourceName = nullptr)
	{
		HRESULT hr = S_OK;

		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(datasize);

		auto device = m_device->GetD3DDevice().Get();

		hr = device->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pResource)
		);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = pResource->SetName(resourceName);
		if (FAILED(hr))
		{
			return hr;
		}

		ComPtr<void> pMappedData;
		pResource->Map(0, nullptr, &pMappedData);
		memcpy(pMappedData.GetAddressOf(), pData, datasize);
		pResource->Unmap(0, nullptr);

		return S_OK;
	}


	HRESULT Renderer::AllocateUAVBuffer(UINT64 bufferSize, ComPtr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON, LPCWSTR resourceName = nullptr)
	{
		HRESULT hr = S_OK;

		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		auto device = m_device->GetD3DDevice().Get();

		hr = device->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			initialResourceState,
			nullptr,
			IID_PPV_ARGS(&pResource)
		);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = pResource->SetName(resourceName);
		if (FAILED(hr))
		{
			return hr;
		}

		return S_OK;
	}
}