/*+===================================================================
  File:      COMMON.H
  Summary:   Common header file that contains common header files and
			 macros used for the Library 
===================================================================+*/
#pragma once

#ifndef  UNICODE
#define UNICODE
#endif // ! UNICODE

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#if defined(CreateWindow)
#undef CreateWindow
#endif

// Windows Runtime Library
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX 12
#include "d3dx12.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

// C RunTime Header Files
#include <stdlib.h>
#include <sstream>
#include <iomanip>

#include <list>
#include <string>
#include <shellapi.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include <atlbase.h>
#include <assert.h>
#include <rpcndr.h>

// Resource
#include "resource.h"

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

using namespace DirectX;
using namespace std;

// window title
constexpr LPCWSTR PSZ_TITLE = L"D3D12_DXR";

// The number of swap chain back buffers
const static size_t MAX_BACK_BUFFER_COUNT = 3;

namespace library
{
    /*S+S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S
    Struct:   DirectionsInput
    Summary:  Data structure that stores keyboard movement data
    S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S-S*/
    struct DirectionsInput
    {
        BOOL bFront;
        BOOL bLeft;
        BOOL bBack;
        BOOL bRight;
        BOOL bUp;
        BOOL bDown;
    };

    /*S+S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S
        Struct:   MouseRelativeMovement
        Summary:  Data structure that stores mouse relative movement data
    S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S-S*/
    struct MouseRelativeMovement
    {
        LONG X;
        LONG Y;
    };

	struct Vertex
	{
        XMFLOAT3 position;
        XMFLOAT3 normal;
	};

	typedef UINT16 Index;

	struct Viewport
	{
		float left;
		float top;
		float right;
		float bottom;
	};

    struct SceneConstantBuffer
    {
        XMMATRIX projectionToWorld;
        XMVECTOR cameraPosition;
        XMVECTOR lightPosition;
        XMVECTOR lightAmbientColor;
        XMVECTOR lightDiffuseColor;
    };

    struct CubeConstantBuffer
    {
        XMFLOAT4 albedo;
    };
}