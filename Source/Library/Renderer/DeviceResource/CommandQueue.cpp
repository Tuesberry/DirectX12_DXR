#include "CommandQueue.h"

namespace library
{
	CommandQueue::CommandQueue()
		: m_commandAllocator{ nullptr }
		, m_commandQueue(nullptr)
		, m_commandList(nullptr)
		, m_backBufferIndex(0)
		, m_backBufferCount(MAX_BACK_BUFFER_COUNT)
		, m_fence(nullptr)
		, m_fenceValues()
		, m_fenceEvent()
		, m_swapChain(nullptr)
	{
	}

	HRESULT CommandQueue::Init(ComPtr<ID3D12Device5> device, shared_ptr<SwapChain> swapChain)
	{
		HRESULT hr = S_OK;

		// swapChain
		m_swapChain = swapChain;

		// 1. create command queue
		D3D12_COMMAND_QUEUE_DESC queueDesc = {
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
		};
		hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.GetAddressOf()));
		if (FAILED(hr))
		{
			return hr;
		}

		// 2. create command allocator for each back buffer that will be rendered to
		for (UINT iBackBuffer = 0; iBackBuffer < m_backBufferCount; iBackBuffer++)
		{
			hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator[iBackBuffer]));
			if (FAILED(hr))
			{
				return hr;
			}
		}

		// 3. create a command list
		hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator[0].Get(), nullptr, IID_PPV_ARGS(&m_commandList));
		if (FAILED(hr))
		{
			return hr;
		}
		hr = m_commandList->Close();
		if (FAILED(hr))
		{
			return hr;
		}

		// 4. create fence 
		hr = device->CreateFence(m_fenceValues[m_backBufferIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
		if (FAILED(hr))
		{
			return hr;
		}
		m_fenceValues[m_backBufferIndex]++;

		m_fenceEvent.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));
		if (!m_fenceEvent.IsValid())
		{
			return E_FAIL;
		}

		return S_OK;
	}

	void CommandQueue::UpdateBackBufferIndex()
	{
		m_backBufferIndex = m_swapChain->GetSwapChain().Get()->GetCurrentBackBufferIndex();
	}

	HRESULT CommandQueue::ExecuteCommandList()
	{
		HRESULT hr = S_OK;

		hr = m_commandList->Close();
		if (FAILED(hr))
		{
			return hr;
		}

		ID3D12CommandList* commandLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(ARRAYSIZE(commandLists), commandLists);

		return S_OK;
	}

	HRESULT CommandQueue::MoveToNextFrame()
	{
		HRESULT hr = S_OK;

		// schedule a signal command in the queue
		const UINT64 currentFenceValue = m_fenceValues[m_backBufferIndex];
		hr = m_commandQueue->Signal(m_fence.Get(), currentFenceValue);
		if (FAILED(hr))
		{
			return hr;
		}

		// update back buffer index
		UpdateBackBufferIndex();

		// If the next frame is not ready to be rendered yet, wait until it is ready.
		if (m_fence->GetCompletedValue() < m_fenceValues[m_backBufferIndex])
		{
			hr = m_fence->SetEventOnCompletion(m_fenceValues[m_backBufferIndex], m_fenceEvent.Get());
			if (FAILED(hr))
			{
				return hr;
			}
			WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);
		}

		// Set the fence value for the next frame.
		m_fenceValues[m_backBufferIndex] = currentFenceValue + 1;

		return S_OK;
	}

	void CommandQueue::WaitForGPU() noexcept
	{
		if (m_commandQueue && m_fence && m_fenceEvent.IsValid())
		{
			// Schedule a Signal command in the GPU queue.
			UINT64 fenceValue = m_fenceValues[m_backBufferIndex];
			if (SUCCEEDED(m_commandQueue->Signal(m_fence.Get(), fenceValue)))
			{
				// Wait until the Signal has been processed.
				if (SUCCEEDED(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent.Get())))
				{
					WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);

					// Increment the fence value for the current frame.
					m_fenceValues[m_backBufferIndex]++;
				}
			}
		}
	}

	void CommandQueue::Prepare(D3D12_RESOURCE_STATES beforeState)
	{
		// Reset command list and allocator.
		m_commandAllocator[m_backBufferIndex]->Reset();
		m_commandList->Reset(m_commandAllocator[m_backBufferIndex].Get(), nullptr);

		if (beforeState != D3D12_RESOURCE_STATE_RENDER_TARGET)
		{
			// Transition the render target into the correct state to allow for drawing into it.
			D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetRenderTargets(m_backBufferIndex).Get(), beforeState, D3D12_RESOURCE_STATE_RENDER_TARGET);
			m_commandList->ResourceBarrier(1, &barrier);
		}
	}

	void CommandQueue::Present(D3D12_RESOURCE_STATES beforeState)
	{
		if (beforeState != D3D12_RESOURCE_STATE_PRESENT)
		{
			// Transition the render target to the state that allows it to be presented to the display.
			D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetRenderTargets(m_backBufferIndex).Get(), beforeState, D3D12_RESOURCE_STATE_PRESENT);
			m_commandList->ResourceBarrier(1, &barrier);
		}

		ExecuteCommandList();

		m_swapChain->GetSwapChain().Get()->Present(1, 0);

		MoveToNextFrame();
	}
}