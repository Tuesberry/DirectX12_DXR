#pragma once

#include "Common.h"

namespace library 
{
	class ConstantBuffer final
	{
    public:
        ConstantBuffer();
        ConstantBuffer(const ConstantBuffer& other) = delete;
        ConstantBuffer(ConstantBuffer&& other) = delete;
        ConstantBuffer& operator=(const ConstantBuffer& other) = delete;
        ConstantBuffer& operator=(ConstantBuffer&& other) = delete;
        ~ConstantBuffer() = default;

        HRESULT Allocate(ComPtr<ID3D12Device> device, UINT bufferSize, LPCWSTR resourceName = nullptr);
        HRESULT Create(ComPtr<ID3D12Device> device, UINT elementCount, UINT size, LPCWSTR resourceName = nullptr);

        ComPtr<ID3D12Resource> GetResource() { return m_resource; }

    private:
        UINT m_elementSize;
        UINT m_elementCount;
        uint8_t* m_mappedConstantData;
        ComPtr<ID3D12Resource> m_resource;

	};
}