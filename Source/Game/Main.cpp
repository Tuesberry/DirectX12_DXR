/*+===================================================================
  File:      MAIN.CPP
  Summary:   This application demonstrates creating a Direct3D 12 device
===================================================================+*/
#include "Common.h"
#include "Game/Game.h"

INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    // Game
	std::unique_ptr<library::Game> game = std::make_unique<library::Game>(L"D3D12_DXR_Initialize");

    // initialize
    if (FAILED(game->Initialize(hInstance, nCmdShow)))
    {
        return 0;
    }

    // run
    return game->Run();

}