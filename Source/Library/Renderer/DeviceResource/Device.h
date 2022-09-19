#pragma once

#include "Common.h"

namespace library
{
	class Device final
	{
    public:
        Device();
        Device(const Device& other) = delete;
        Device(Device&& other) = delete;
        Device& operator=(const Device& other) = delete;
        Device& operator=(Device&& other) = delete;
        ~Device() = default;

        HRESULT Init();

        ComPtr<ID3D12Device5> GetD3DDevice() { return m_d3dDevice; }
        ComPtr<IDXGIFactory> GetDXGIFactory() { return m_dxgiFactory; }
        ComPtr<IDXGIAdapter1> GetAdapter() { return m_adapter; }

    private:
        // device
        ComPtr<ID3D12Device5> m_d3dDevice;

        // factory
        ComPtr<IDXGIFactory> m_dxgiFactory;

        // adpater
        ComPtr<IDXGIAdapter1> m_adapter;
        UINT m_adapterID;
        std::wstring m_adapterDescription;
	};
}