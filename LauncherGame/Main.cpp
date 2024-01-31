#include "Core/Core.hpp"
#include "Core/LoggerManager.hpp"
#include "Core/Platform.hpp"
#include "Input/InputManager.hpp"
#include "Launcher/Sources/CommandLineParser.hpp"
#include "Launcher/Sources/LogLauncher.hpp"
#include "LauncherGame/Sources/InputExtractor.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <format>
#include <memory>
#include <shellapi.h>
#include <string>

static void             HandleCommandLine(CommandLineParseResults<wchar_t> const& cli) noexcept;
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
static HWND             InitWindow(HINSTANCE hInstance, int nCmdShow) noexcept;
static void             InitPlatformInput(HWND windowHandle) noexcept;
static LRESULT          HandleRawInput(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

int wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nShowCmd)
{
    {
        int        argc{};
        wchar_t**  argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        auto const r    = CommandLineParser<wchar_t>{}.parse(argv);
        HandleCommandLine(r);
        LocalFree(argv); // CLI Parser returns a `view` to the command line parameters
    }

    HWND hwnd = InitWindow(hInstance, nShowCmd);
    InitPlatformInput(hwnd);

    MSG msg = {};
    while(true)
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD))
        {
            if(msg.message == WM_QUIT) return -1;
            DispatchMessage(&msg);
        }
    }
}

void HandleCommandLine(CommandLineParseResults<wchar_t> const& cli) noexcept
{
    using enum Core::LogLevel;

    constexpr std::array<Core::LogLevel, 6> levels{
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal,
    };

    constexpr std::array<std::wstring_view, 6> levelStrings{
        L"Trace",
        L"Debug",
        L"Info",
        L"Warning",
        L"Error",
        L"Fatal",
    };

    auto const levelBegin = levelStrings.cbegin();
    auto const levelEnd   = levelStrings.cend();
    for(auto const& [K, V]: cli)
    {
        if(!V.empty() && K.starts_with(L"Log"))
        {
            if(auto it = std::ranges::find(levelStrings, K); it != levelEnd)
            {
                Core::LoggerManager::changeLoggerLevel(K, levels[it - levelBegin]);
            }
        }
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch(uMsg)
    {
    case WM_INPUT:
        return HandleRawInput(hwnd, uMsg, wParam, lParam);
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

HWND InitWindow(HINSTANCE hInstance, int nCmdShow) noexcept
{
    constexpr auto className = L"GE_GAME_WINDOW_100CAF0C-F24C-4D77-A1D8-7182C0F57CF2";

    WNDCLASS wc      = {};
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = className;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        className,
        L"Game Engine - Game Window",
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

void InitPlatformInput(HWND windowHandle) noexcept
{
    RAWINPUTDEVICE Rid[2]{};

    Rid[0].usUsagePage = 0x01; // HID_USAGE_PAGE_GENERIC
    Rid[0].usUsage     = 0x02; // HID_USAGE_GENERIC_MOUSE
    Rid[0].dwFlags     = 0;    // If RIDEV_NOLEGACY, won't be able to move the window
    Rid[0].hwndTarget  = windowHandle;

    Rid[1].usUsagePage = 0x01; // HID_USAGE_PAGE_GENERIC
    Rid[1].usUsage     = 0x06; // HID_USAGE_GENERIC_KEYBOARD
    Rid[1].dwFlags     = 0;    // If RIDEV_NOLEGACY, won't be able to move the window
    Rid[1].hwndTarget  = windowHandle;

    if(RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
    {
        LOG(LogLauncher, Core::LogLevel::Fatal, L"Failed to initialize platform input: {}", Core::getLastPlatformError());
        std::abort();
    }
}

LRESULT HandleRawInput(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    double const timestamp = double(std::chrono::steady_clock::now().time_since_epoch().count());

    if(GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUTSINK)
        return 0;

    RAWINPUT raw{};
    UINT     dwSize{};

    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
    if(dwSize > sizeof(raw))
    {
        LOG(LogLauncher, Core::LogLevel::Error, L"GetRawInputData(...) size `{}` != sizeof(RAWINPUT) `{}`", dwSize, sizeof(raw));
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    if(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
    {
        LOG(LogLauncher, Core::LogLevel::Error, L"GetRawInputData didn't return the correct size!");
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    using enum Input::EventTypes;

    static auto& inputManager = Input::InputManager::instance();

    using type_t = std::underlying_type_t<Input::EventTypes>;
    type_t type{};
    type |= inputManager.canHandleMouse() ? Mouse : 0;
    type |= inputManager.canHandleKeyboard() ? Keyboard : 0;
    type |= inputManager.canHandleCharacters() ? Character : 0;

    Launcher::InputData input = Launcher::extractInput(raw, static_cast<Input::EventTypes>(type));

    if(input.type & Mouse)
    {
        if(input.mouse.type == Launcher::MouseType::Button)
        {
            if(input.mouse.action == Launcher::ActionType::Press)
            {
                inputManager.dispatchMouseDown(timestamp, input.mouse.button);
            }
            else
            {
                inputManager.dispatchMouseUp(timestamp, input.mouse.button);
            }
        }
        else if(input.mouse.type == Launcher::MouseType::Movement)
        {
            inputManager.dispatchMouseMove(timestamp, input.mouse.dx, input.mouse.dy);
        }
        else if(input.mouse.type == Launcher::MouseType::Wheel)
        {
            inputManager.dispatchMouseWheel(timestamp, input.mouse.isVerticalScroll, input.mouse.ticks);
        }
    }

    if(input.type & Keyboard)
    {
        if(input.keyboard.action == Launcher::ActionType::Press)
            inputManager.dispatchKeyDown(timestamp, input.keyboard.scanCode);
        else
            inputManager.dispatchKeyUp(timestamp, input.keyboard.scanCode);
    }

    if(input.type & Character)
    {
        inputManager.dispatchCharacter(timestamp, input.keyboard.character);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
