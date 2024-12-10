#include <Application/LogApplication.h>
#include <Core/Assert/Assert.h>
// #include <Input/Base/Translator.h>
// #include <Render/RenderingSystem.h>
// #include <Render/Shaders/Shaders.h>
#include <Windows.h>
#include <hidusage.h>

wchar_t const CLASS_NAME[]           = L"GE_MAIN_WINDOW_CLASS";
HINSTANCE     hInstance{};
HWND          MainWindowHnd{};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool             CreateRenderingWindow(wchar_t const* Title, int const Width, int const Height);
// bool             InitRawInput();

struct EngineShutdownOnExit
{
  ~EngineShutdownOnExit()
  {
    if (MainWindowHnd)
      DestroyWindow(MainWindowHnd);

    UnregisterClass(CLASS_NAME, hInstance);
  }
};

int WINAPI wWinMain(_In_ HINSTANCE hInstance_, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
  (void)hPrevInstance;
  (void)lpCmdLine;

  hInstance = hInstance_;

  EngineShutdownOnExit _;

  if (!verifyf(CreateRenderingWindow(L"Game Engine - OpenGL Drawing", 640, 480), "Couldn't create main window."))
    return -1;

  // if (!verifyf(Render::RenderingSystem::Init(MainWindowHnd, false), "Couldn't initialize Rendering System."))
  //   return -2;

  // if (!verifyf(InitRawInput(), "Couldn't initialize raw input."))
  //   return -3;

  ShowWindow(MainWindowHnd, nShowCmd);

  while (true)
  {
    MSG msg{};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)
        return (i32)msg.wParam;

      if (msg.message != WM_INPUT)
        TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  // TODO: forward to input subsystem
  // Input::NativeEvent const nev{
  //     .windows = {
  //         .Hwnd_    = hwnd,
  //         .WParam_  = wParam,
  //         .LParam_  = lParam,
  //         .Message_ = uMsg,
  //     },
  // };

  switch (uMsg)
  {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  case WM_SIZE:
    // Render::RenderingSystem::ResizeViewport(LOWORD(lParam), HIWORD(lParam));
    return 0;

  case WM_INPUT: {
    // TODO: forward to input subsystem
    // Input::InputEvent ev;
    // if (Input::TranslateNativeEvent(nev, ev))
    // {
    // }
    break;
  }
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// TODO: Move into the input subsystem
// bool InitRawInput()
// {
//   RAWINPUTDEVICE rid[2] = {};
//
//   rid[0].dwFlags     = 0;
//   rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
//   rid[0].usUsage     = HID_USAGE_GENERIC_MOUSE;
//   rid[0].hwndTarget  = NULL;
//
//   rid[1].dwFlags     = 0;
//   rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
//   rid[1].usUsage     = HID_USAGE_GENERIC_KEYBOARD;
//   rid[1].hwndTarget  = NULL;
//
//   return RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE));
// }

bool CreateRenderingWindow(wchar_t const* Title, int const Width, int const Height)
{
  DWORD dwExStyle  = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
  DWORD dwStyle    = WS_OVERLAPPEDWINDOW;
  RECT  WindowRect = {
       .left   = 0,
       .top    = 0,
       .right  = Width,
       .bottom = Height,
  };
  WNDCLASS wc{
      .style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
      .lpfnWndProc   = (WNDPROC)WindowProc,
      .hInstance     = hInstance,
      .hIcon         = LoadIcon(NULL, IDI_WINLOGO),
      .hCursor       = LoadCursor(NULL, IDC_ARROW),
      .lpszClassName = CLASS_NAME,
  };

  if (!RegisterClass(&wc))
    return false;

  AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle); // Adjust Window To True Requested Size

  MainWindowHnd = CreateWindowEx(dwExStyle,
                                 CLASS_NAME,
                                 Title,
                                 dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                 0, 0,
                                 WindowRect.right - WindowRect.left,
                                 WindowRect.bottom - WindowRect.top,
                                 NULL,
                                 NULL,
                                 hInstance,
                                 NULL);
  if (!MainWindowHnd)
    return false;

  SetForegroundWindow(MainWindowHnd);
  SetFocus(MainWindowHnd);

  return true;
}
