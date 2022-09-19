#include "Device.h"

namespace library
{
	Device::Device()
		: m_d3dDevice(nullptr)
		, m_dxgiFactory(nullptr)
		, m_adapter(nullptr)
		, m_adapterDescription()
		, m_adapterID()
	{
	}

	HRESULT Device::Init()
	{
		HRESULT hr = S_OK;

		// create dxgi factory
		hr = CreateDXGIFactory(IID_PPV_ARGS(&m_dxgiFactory));
		if (FAILED(hr))
		{
			return hr;
		}

		// check DXGI 1.6 support
		ComPtr<IDXGIFactory6> factory6;
		hr = m_dxgiFactory.As(&factory6);
		if (FAILED(hr))
		{
			return hr;
		}

		// get adapter using DXGIfactory
		ComPtr<IDXGIAdapter1> adapter;
		for (UINT adapterId = 0; DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(adapterId, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)); ++adapterId)
		{
			DXGI_ADAPTER_DESC1 desc;
			hr = adapter->GetDesc1(&desc);
			if (FAILED(hr))
			{
				return hr;
			}

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the basic render driver adapter
				continue;
			}

			// check to see if the adpater supports Direct3D 12, but don't create the actual device yet
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
			{
				m_adapterID = adapterId;
				m_adapterDescription = desc.Description;
				break;
			}
		}

		// detach returns pointer to the interface represented by the ComPtr object
		m_adapter = adapter.Detach();

		// create device
		hr = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice));
		if (FAILED(hr))
		{
			return hr;
		}

		return S_OK;
	}
}