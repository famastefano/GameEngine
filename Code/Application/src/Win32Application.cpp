#include <Application/LogApplication.h>
#include <Core/Assert/Assert.h>
#include <Input/Base/Translator.h>
#include <Windows.h>
#include <hidusage.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool InitRawInput();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
  wchar_t const CLASS_NAME[] = L"GE_MAIN_WINDOW_CLASS";

  WNDCLASS wc{};
  wc.lpfnWndProc   = WindowProc;
  wc.hInstance     = hInstance;
  wc.lpszClassName = CLASS_NAME;

  RegisterClass(&wc);

  // Create the window.

  HWND hwnd = CreateWindowEx(
      0,                           // Optional window styles.
      CLASS_NAME,                  // Window class
      L"Learn to Program Windows", // Window text
      WS_OVERLAPPEDWINDOW,         // Window style

      // Size and position
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

      NULL,      // Parent window
      NULL,      // Menu
      hInstance, // Instance handle
      NULL       // Additional application data
  );
  if (!verifyf(hwnd, "Couldn't create main window."))
    return -1;

  if (!verifyf(InitRawInput(), "Couldn't initialize raw input."))
    return -2;

  ShowWindow(hwnd, nCmdShow);

  bool shutdown = false;
  while (!shutdown)
  {
    MSG msg{};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)
      {
        shutdown = true;
        break;
      }

      if (msg.message != WM_INPUT)
        TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  Input::NativeEvent const nev{
      .windows = {
          .Hwnd_    = hwnd,
          .WParam_  = wParam,
          .LParam_  = lParam,
          .Message_ = uMsg,
      },
  };

  switch (uMsg)
  {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC         hdc = BeginPaint(hwnd, &ps);
    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
    EndPaint(hwnd, &ps);
    return 0;
  }
  case WM_INPUT: {
    Input::InputEvent ev;
    if (Input::TranslateNativeEvent(nev, ev))
    {
      char const* kinds[] = {
          "Mouse_Press",
          "Mouse_Release",
          "Mouse_Move",
          "Mouse_Wheel",
          "Key_Press",
          "Key_Release",
      };
      if (ev.Kind_ == Input::Kind::Key_Press || ev.Kind_ == Input::Kind::Key_Release)
      {
        if (ev.Keyboard_.Char_)
          GE_LOG(LogApplication, Core::Verbosity::Debug, "%s %u %u %C", kinds[(u8)ev.Kind_], ev.Keyboard_.ScanCode_, ev.Keyboard_.NativeVirtualKey_, ev.Keyboard_.Char_);
        else
          GE_LOG(LogApplication, Core::Verbosity::Debug, "%s %u %u", kinds[(u8)ev.Kind_], ev.Keyboard_.ScanCode_, ev.Keyboard_.NativeVirtualKey_);
      }
      else
      {
        char const* buttons[] = {
            "NoButton",
            "Mouse0",
            "Mouse1",
            "Mouse2",
            "Mouse3",
            "Mouse4",
            "MouseWheel",
        };
        GE_LOG(LogApplication, Core::Verbosity::Debug,
               "%s %s (%d, %d) %d",
               kinds[(u8)ev.Kind_],
               buttons[(i16)ev.Mouse_.Button_],
               (i32)ev.Mouse_.MoveX_,
               (i32)ev.Mouse_.MoveY_,
               (i32)ev.Mouse_.WheelY);
      }
    }
    break;
  }
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool InitRawInput()
{
  RAWINPUTDEVICE rid[2] = {};

  rid[0].dwFlags     = 0;
  rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
  rid[0].usUsage     = HID_USAGE_GENERIC_MOUSE;
  rid[0].hwndTarget  = NULL;

  rid[1].dwFlags     = 0;
  rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
  rid[1].usUsage     = HID_USAGE_GENERIC_KEYBOARD;
  rid[1].hwndTarget  = NULL;

  return RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE));
}
