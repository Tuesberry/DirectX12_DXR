#include "Renderer.h"

namespace library
{
	Renderer::Renderer()
		:m_device(nullptr)
		,m_commandQueue(nullptr)
		,m_swapChain(nullptr)
		,m_backBuffers{}
		,m_commandList(nullptr)
		,m_commandAllocator{}
		,m_rtvDescriptorHeap(nullptr)
		,m_fence(nullptr)
		,m_rootSignature(nullptr)
		,m_pipelineState(nullptr)
		,m_vertexBuffer(nullptr)
		,m_vertexBufferView{}
		,m_viewport{0, 0, 0.0f, 0.0f, 0.0f, 1.0f}
		,m_scissorRect{}
		,m_rtvHandle{}
		,m_rtvDescriptorSize(0)
		,m_currentBackBufferIndex(0)
		,m_fenceValue(0)
		,m_fenceEvent(INVALID_HANDLE_VALUE)
	{
	}

	HRESULT Renderer::Initialize(_In_ HWND hWnd)
	{
		HRESULT hr = S_OK;

		RECT rc;
		GetClientRect(hWnd, &rc);
		UINT uWidth = static_cast<UINT>(rc.right - rc.left);
		UINT uHeight = static_cast<UINT>(rc.bottom - rc.top);

		// set viewport
		m_viewport.Width = static_cast<float>(uWidth);
		m_viewport.Height = static_cast<float>(uHeight);

		// set rect
		m_scissorRect = CD3DX12_RECT(0, 0, uWidth, uHeight);

		// 1. create the device
		// create dxgiFactory
		ComPtr<IDXGIFactory> dxgiFactory(nullptr);
		hr = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}

		// createDevice
		hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_device.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}

		// 2. Create a direct command queue
		D3D12_COMMAND_QUEUE_DESC queueDesc = {
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
		};
		hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}

		// 3. create the swap chain
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {
			.BufferDesc = {
				.Width = uWidth,
				.Height = uHeight,
				.RefreshRate = {
					.Numerator = 60,
					.Denominator = 1
				},
				.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
				.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
				.Scaling = DXGI_MODE_SCALING_UNSPECIFIED
			},
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = g_numFrameBuffers,
			.OutputWindow = hWnd,
			.Windowed = true,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		};

		ComPtr<IDXGISwapChain> swapChain(nullptr);
		hr = dxgiFactory->CreateSwapChain(m_commandQueue.Get(), &swapChainDesc, swapChain.GetAddressOf());
		if (FAILED(hr))
		{
			return hr;
		}

		hr = swapChain.As(&m_swapChain);
		if (FAILED(hr))
		{
			return hr;
		}
		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

		// 4. Create the Descriptor Heap
		// rtv descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = g_numFrameBuffers,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0
		};
		hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtvDescriptorHeap.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}
		// get the size of a descriptor in this heap
		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// 5. Create frame resources
		// get a handle to the first descriptor in the descriptor heap
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		// create a RTV for each buffer
		for (UINT i = 0; i < g_numFrameBuffers; ++i)
		{
			hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_backBuffers[i].GetAddressOf()));
			if (FAILED(hr))
			{
				return hr;
			}
			m_device->CreateRenderTargetView(m_backBuffers[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}

		// 6. Create the Command Allocator
		hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_commandAllocator.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}

		// 7. Create an empty root signature
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.GetAddressOf(), error.GetAddressOf());
		if (FAILED(hr))
		{
			return hr;
		}
		hr = m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}

		// 8. Create the pipeline state, which includes compiling and loading shaders
		// d3dcompilefromfile
		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;
		hr = D3DCompileFromFile(L"../Library/Shaders/VertexShader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, vertexShader.GetAddressOf(), nullptr);
		if (FAILED(hr))
		{
			return hr;
		}
		hr = D3DCompileFromFile(L"../Library/Shaders/PixelShader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, pixelShader.GetAddressOf(), nullptr);
		if (FAILED(hr))
		{
			return hr;
		}

		// define the vertex input layout
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// describe and create the graphics pipeline state object
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {
			.pRootSignature = m_rootSignature.Get(),
			.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get()),
			.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get()),
			.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
			.SampleMask = UINT_MAX,
			.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
			.DepthStencilState = {
				.DepthEnable = FALSE,
				.StencilEnable = FALSE
			},
			.InputLayout = {
				inputElementDescs, _countof(inputElementDescs)
			},
			.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			.NumRenderTargets = 1,
			.RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM, },
			.SampleDesc = {
				.Count = 1,
			},
		};
		hr = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_pipelineState.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}

		// 9. Create the Command list
		hr = m_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_commandAllocator.Get(),
			m_pipelineState.Get(),
			IID_PPV_ARGS(m_commandList.GetAddressOf())
		);
		if (FAILED(hr))
		{
			return hr;
		}
		// close command list
		m_commandList->Close();

		// 10. create the vertex buffer
		float aspect_ratio = static_cast<float>(uWidth) / static_cast<float>(uHeight);
		Vertex triangleVertices[] = {
				{ { 0.0f, 0.25f * aspect_ratio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
				{ { 0.25f, -0.25f * aspect_ratio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
				{ { -0.25f, -0.25f * aspect_ratio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		};

		// create a resource heap
		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(triangleVertices));
		hr = m_device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(m_vertexBuffer.GetAddressOf())
		);
		if (FAILED(hr))
		{
			return hr;
		}

		// copy the triangle data to the vertex buffer
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);
		hr = m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		if (FAILED(hr))
		{
			return hr;
		}
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_vertexBuffer->Unmap(0, nullptr);

		// initialize the vertex buffer view
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes = sizeof(Vertex);
		m_vertexBufferView.SizeInBytes = sizeof(triangleVertices);

		// 11. create synchronization object
		// create the fence
		hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}
		m_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		// wait for the command list to execute
		WaitForPreviousFrame();
	}

	void Renderer::WaitForPreviousFrame()
	{
		// Signal and increment the fence value.
		const UINT64 fence = m_fenceValue;
		m_commandQueue->Signal(m_fence.Get(), fence);
		m_fenceValue++;

		// Wait until the previous frame is finished.
		if (m_fence->GetCompletedValue() < fence)
		{
			m_fence->SetEventOnCompletion(fence, m_fenceEvent);
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}

		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	void Renderer::Update()
	{

	}

	void Renderer::Render()
	{
		m_commandAllocator->Reset();
		m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get());

		// Clear the render target
		// set necessary state
		m_commandList->RSSetViewports(1, &m_viewport);
		m_commandList->RSSetScissorRects(1, &m_scissorRect);
		m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

		// indicate that the back buffer will be used as a render target
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_backBuffers[m_currentBackBufferIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList->ResourceBarrier(1, &barrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBackBufferIndex, m_rtvDescriptorSize);
		m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// record commands
		m_commandList->ClearRenderTargetView(rtvHandle, Colors::MidnightBlue, 0, nullptr);
		m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
		m_commandList->DrawInstanced(3, 1, 0, 0);

		// indicate that the back buffer will now be used to present
		CD3DX12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
			m_backBuffers[m_currentBackBufferIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1, &barrier2);
		m_commandList->Close();

		// execute the command list
		ID3D12CommandList* commandListArr[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(commandListArr), commandListArr);
		
		// present the frame
		m_swapChain->Present(1, 0);
		
		WaitForPreviousFrame();
	}
}