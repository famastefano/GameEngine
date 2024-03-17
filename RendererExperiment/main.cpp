#include "Core/Core.hpp"
#include "Core/LoggerManager.hpp"
#include "Core/Platform.hpp"
#include "Input/InputManager.hpp"
#include "Renderer/Engine.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
static HWND             InitWindow(HINSTANCE hInstance, int nCmdShow) noexcept;
static void             InitSwapChain(HWND hwnd) noexcept;

Renderer::Engine renderingEngine;

int wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nShowCmd)
{
    HWND hwnd = InitWindow(hInstance, nShowCmd);

    InitSwapChain(hwnd);

    MSG msg = {};
    while(true)
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD))
        {
            if(msg.message == WM_QUIT) return -1;
            DispatchMessage(&msg);
        }

        renderingEngine.tick();
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch(uMsg)
    {
    case WM_KEYDOWN:
    {
        switch(wParam)
        {
        case VK_LEFT:
        case VK_RIGHT:
        case VK_UP:
        case VK_DOWN:
            break;
        }
    }
    case WM_SIZE:
    {
        if(wParam == SIZE_RESTORED)
            renderingEngine.resizeOutput(LOWORD(lParam), HIWORD(lParam));
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

HWND InitWindow(HINSTANCE hInstance, int nCmdShow) noexcept
{
    constexpr auto className = L"RendererExperiment_DEBUG";

    WNDCLASS wc      = {};
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = className;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        className,
        L"Game Engine - Renderer Experiment",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);

    if(hwnd != NULL)
        ShowWindow(hwnd, nCmdShow);

    return hwnd;
}

void InitSwapChain(HWND hwnd) noexcept
{
    auto adapter = renderingEngine.adapters().front();
    auto output  = renderingEngine.outputs(adapter).front();

    RECT clientArea{0, 0, 1, 1};
    GetClientRect(hwnd, &clientArea);

    Renderer::Engine::SwapChainOptions opt;
    opt.windowHandle = hwnd;
    opt.width        = clientArea.right - clientArea.left;
    opt.height       = clientArea.bottom - clientArea.top;
    opt.refreshRate  = 60;

    if(!renderingEngine.init(adapter, output, opt))
        assertf(false, L"Failed to initialize the swap chain");
}
