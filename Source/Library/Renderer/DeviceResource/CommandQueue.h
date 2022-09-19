#pragma once

#include "Common.h"
#include "SwapChain.h"

namespace library
{
	class CommandQueue final
	{
    public:
        CommandQueue();
        CommandQueue(const CommandQueue& other) = delete;
        CommandQueue(CommandQueue&& other) = delete;
        CommandQueue& operator=(const CommandQueue& other) = delete;
        CommandQueue& operator=(CommandQueue&& other) = delete;
        ~CommandQueue() = default;

        HRESULT Init(ComPtr<ID3D12Device5> device, shared_ptr<SwapChain> swapChain);
        void UpdateBackBufferIndex();

        HRESULT ExecuteCommandList();
        HRESULT MoveToNextFrame();
        void WaitForGPU() noexcept;
        void Prepare(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_PRESENT);
        void Present(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_RENDER_TARGET);

        ComPtr<ID3D12CommandQueue> GetCommandQueue() { return m_commandQueue; }
        ComPtr<ID3D12GraphicsCommandList4> GetCommandList() { return m_commandList; }
        ComPtr<ID3D12CommandAllocator> GetCommandAllocator() { return m_commandAllocator[m_backBufferIndex]; }
        UINT GetBackBufferIndex() { return m_backBufferIndex; }
    private:
        // command queue, list, allocator
        ComPtr<ID3D12CommandQueue> m_commandQueue;
        ComPtr<ID3D12GraphicsCommandList4> m_commandList;
        ComPtr<ID3D12CommandAllocator> m_commandAllocator[MAX_BACK_BUFFER_COUNT];
        
        // back buffer index
        UINT m_backBufferIndex;
        UINT m_backBufferCount;

        // fence object
        ComPtr<ID3D12Fence> m_fence;
        UINT m_fenceValues[MAX_BACK_BUFFER_COUNT];
        Wrappers::Event m_fenceEvent;

        // swapChain
        shared_ptr<SwapChain> m_swapChain;
	};
}