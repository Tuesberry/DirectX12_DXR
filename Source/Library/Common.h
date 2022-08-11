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

// STL Headers
#include <algorithm>
#include <cassert>
#include <chrono>
#include <memory>
#include <exception>
#include <shellapi.h> // for CommandLineToArgvW

#include "resource.h"

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

using namespace DirectX;

constexpr LPCWSTR PSZ_TITLE = L"D3D12_DXR";

// The number of swap chain back buffers
const uint8_t g_NumFrameBuffers = 3;

const uint32_t g_ClientWidth = 1280;
const uint32_t g_ClientHeight = 720;
