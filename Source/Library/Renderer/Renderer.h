#pragma once

#include "Common.h"
// root signature
#include "RootSignature/GlobalRootSignature.h"
#include "RootSignature/LocalRootSignature.h"
// renderable
#include "GameObject/Cube.h"
// device resources
#include "Renderer/DeviceResource/DescriptorHeap.h"
#include "Renderer/DeviceResource/Device.h"
#include "Renderer/DeviceResource/CommandQueue.h"
#include "Renderer/DeviceResource/DepthStencilBuffer.h"
#include "Renderer/DeviceResource/SwapChain.h"

namespace library
{
	class Renderer final
	{
	public:
		Renderer(UINT width = 800, UINT height = 600, std::wstring name = PSZ_TITLE);
		Renderer(const Renderer& other) = delete;
		Renderer(Renderer&& other) = delete;
		Renderer& operator=(const Renderer& other) = delete;
		Renderer& operator=(Renderer&& other) = delete;
		~Renderer() = default;

		// Messages
		HRESULT Initialize(HWND hWnd);
		void Update();
		void Render();
		
	private:
		// Device Resources
		HRESULT CreateDeviceResources();

		// Raytracing device resources
		HRESULT CreateRaytracingDeviceResources();
		HRESULT CreateRaytracingPipelineStateObject();
		HRESULT BuildAccelerationStructres();
		HRESULT BuildShaderTables();
		void CreateRaytracingOutputResource();
		void DoRaytracing();
		void CopyRaytracingOutputToBackbuffer();

		// allocate buffer
		HRESULT AllocateUploadBuffer(void* pData, UINT64 datasize, ComPtr<ID3D12Resource> pResource, LPCWSTR resourceName = nullptr);
		HRESULT AllocateUAVBuffer(UINT64 bufferSize, ComPtr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON, LPCWSTR resourceName = nullptr);

		static const UINT FrameCount = 3;

		// Viewport dimensions.
		HWND m_window;
		Viewport m_viewport;

		// D3D device resources
		shared_ptr<Device> m_device;
		shared_ptr<CommandQueue> m_commandQueue;
		shared_ptr<DepthStencilBuffer> m_depthStencilBuffer;
		shared_ptr<SwapChain> m_swapChain;
			
		// Window title.
		std::wstring m_title;

		// DirectX Raytracing (DXR) State Object
		ComPtr<ID3D12StateObject> m_dxrStateObject;

		// Root signatures
		shared_ptr<GlobalRootSignature> m_raytracingGlobalRootSignature;
		shared_ptr<LocalRootSignature> m_raytracingLocalRootSignature;

		// Descriptors
		shared_ptr<DescriptorHeap> m_descriptorHeap;

		// Raytracing scene
		SceneConstantBuffer m_sceneCB[FrameCount];
		CubeConstantBuffer m_cubeCB;

		// Geometry
		shared_ptr<Cube> m_cube;

		// Acceleration structure
		ComPtr<ID3D12Resource> m_accelerationStructure;
		ComPtr<ID3D12Resource> m_bottomLevelAccelerationStructure;
		ComPtr<ID3D12Resource> m_topLevelAccelerationStructure;

		// Raytracing output
		ComPtr<ID3D12Resource> m_raytracingOutput;
		D3D12_GPU_DESCRIPTOR_HANDLE m_raytracingOutputResourceUAVGpuDescriptor;
		UINT m_raytracingOutputResourceUAVDescriptorHeapIndex;

		// Shader tables
		static const wchar_t* m_hitGroupName;
		static const wchar_t* m_raygenShaderName;
		static const wchar_t* m_closestHitShaderName;
		static const wchar_t* m_missShaderName;
		ComPtr<ID3D12Resource> m_missShaderTable;
		ComPtr<ID3D12Resource> m_hitGroupShaderTable;
		ComPtr<ID3D12Resource> m_rayGenShaderTable;
	};
}