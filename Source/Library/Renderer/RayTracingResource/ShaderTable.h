#pragma once

#include "Common.h"
#include "ShaderRecord.h"

namespace library
{
	class ShaderTable
	{
    public:
        ShaderTable(ComPtr<ID3D12Device> device, UINT numShaderRecords, UINT shaderRecordSize, LPCWSTR resourceName = nullptr);

        ShaderTable() = delete;
        ShaderTable(const ShaderTable& other) = delete;
        ShaderTable(ShaderTable&& other) = delete;
        ShaderTable& operator=(const ShaderTable& other) = delete;
        ShaderTable& operator=(ShaderTable&& other) = delete;
        ~ShaderTable() = default;

        HRESULT PushBack(const ShaderRecord& shaderRecord);
	};
}