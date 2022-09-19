#pragma once

#include "Common.h"

namespace library
{
	class ShaderRecord
	{
    public:
        ShaderRecord(void* pShaderIdentifier, UINT shaderIdentifierSize);
        ShaderRecord(void* pShaderIdentifier, UINT shaderIdentifierSize, void* pLocalRootArguments, UINT localRootArgumentsSize);

        ShaderRecord() = delete;
        ShaderRecord(const ShaderRecord& other) = delete;
        ShaderRecord(ShaderRecord&& other) = delete;
        ShaderRecord& operator=(const ShaderRecord& other) = delete;
        ShaderRecord& operator=(ShaderRecord&& other) = delete;
        ~ShaderRecord() = default;

        void CopyTo(void* dest) const;

        struct PointerWithSize
        {
            ComPtr<void> m_ptr;
            UINT m_size;

            PointerWithSize() : m_ptr(nullptr), m_size(0) {}
            PointerWithSize(ComPtr<void> ptr, UINT size): m_ptr(ptr), m_size(size) {}
        };

        PointerWithSize m_shaderIdentifier;
        PointerWithSize m_localRootArguments;
	};
}