#pragma once

#include "Common.h"

#include "Renderer/Renderer.h"
#include "Window/MainWindow.h"

namespace library
{
	class Game final
	{
    public:
        Game(_In_ PCWSTR pszGameName);
        Game(const Game& other) = delete;
        Game(Game&& other) = delete;
        Game& operator=(const Game& other) = delete;
        Game& operator=(Game&& other) = delete;
        ~Game() = default;

        HRESULT Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow);
        INT Run();

        //PCWSTR GetGameName() const;
        //std::unique_ptr<Window>& GetWindow();
        //std::unique_ptr<Renderer>& GetRenderer();
    private:
        PCWSTR m_pszGameName;
        std::unique_ptr<MainWindow> m_Window;
        std::unique_ptr<Renderer> m_renderer;
	};
}