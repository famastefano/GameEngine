#include <Application/LogApplication.h>
#include <Core/Assert/Assert.h>
#include <Input/Base/Translator.h>
#include <Windows.h>
#include <glad/glad.h>
#include <hidusage.h>

wchar_t const CLASS_NAME[]  = L"GE_MAIN_WINDOW_CLASS";
HDC           GDICtx        = NULL;
HGLRC         Ctx           = NULL;
HWND          MainWindowHnd = NULL;
HINSTANCE     hInstance;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool             CreateOpenGLWindow(wchar_t const* Title, int const Width, int const Height, int const Depth, bool const Fullscreen);
bool             InitRawInput();
bool             InitRawInput();
void             ResizeViewport(int width, int height);
bool             InitOpenGL();
void             Render();
void             ShutdownOpenGL();

struct EngineShutdownOnExit
{
  ~EngineShutdownOnExit()
  {
    ShutdownOpenGL();
  }
};

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
  (void)hPrevInstance;

  ::hInstance = hInstance;

  EngineShutdownOnExit _;

  if (!verifyf(CreateOpenGLWindow(L"Game Engine - OpenGL Rendering", 640, 480, 8, false), "Couldn't create main window."))
    return -1;

  if (!verifyf(InitOpenGL(), "Couldn't initialize OpenGL."))
    return -2;

  if (!verifyf(InitRawInput(), "Couldn't initialize raw input."))
    return -3;

  ShowWindow(MainWindowHnd, nShowCmd);

  while (true)
  {
    MSG msg{};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)
        return msg.wParam;

      if (msg.message != WM_INPUT)
        TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    Render();
    SwapBuffers(GDICtx);
  }
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

  case WM_SIZE:
    ResizeViewport(LOWORD(lParam), HIWORD(lParam));
    return 0;

  case WM_INPUT: {
    Input::InputEvent ev;
    if (Input::TranslateNativeEvent(nev, ev))
    {
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

void ResizeViewport(int width, int height)
{
  glViewport(0, 0, width, height); // Reset The Current Viewport
}

bool InitOpenGL()
{
  if (!gladLoadGL())
    return false;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  return true;
}

void Render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ShutdownOpenGL()
{
  ChangeDisplaySettings(NULL, 0);
  ShowCursor(TRUE);

  if (Ctx)
  {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(Ctx);
  }

  if (GDICtx)
    ReleaseDC(MainWindowHnd, GDICtx);

  if (MainWindowHnd)
    DestroyWindow(MainWindowHnd);

  UnregisterClass(CLASS_NAME, hInstance);
}

bool CreateOpenGLWindow(wchar_t const* Title, int const Width, int const Height, int const Depth, bool const Fullscreen)
{
  GLuint PixelFormat{};
  DWORD  dwExStyle{};
  DWORD  dwStyle{};
  RECT   WindowRect = {
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

  if (Fullscreen)
  {
    DEVMODE dmScreenSettings{};
    dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth  = Width;
    dmScreenSettings.dmPelsHeight = Height;
    dmScreenSettings.dmBitsPerPel = Depth;
    dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
      return false;
  }

  if (Fullscreen)
  {
    dwExStyle = WS_EX_APPWINDOW; // Window Extended Style
    dwStyle   = WS_POPUP;        // Windows Style
  }
  else
  {
    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // Window Extended Style
    dwStyle   = WS_OVERLAPPEDWINDOW;                // Windows Style
  }

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

  static PIXELFORMATDESCRIPTOR pfd = // pfd Tells Windows How We Want Things To Be
      {
          sizeof(PIXELFORMATDESCRIPTOR), // Size Of This Pixel Format Descriptor
          1,                             // Version Number
          PFD_DRAW_TO_WINDOW |           // Format Must Support Window
              PFD_SUPPORT_OPENGL |       // Format Must Support OpenGL
              PFD_DOUBLEBUFFER,          // Must Support Double Buffering
          PFD_TYPE_RGBA,                 // Request An RGBA Format
          Depth,                         // Select Our Color Depth
          0, 0, 0, 0, 0, 0,              // Color Bits Ignored
          0,                             // No Alpha Buffer
          0,                             // Shift Bit Ignored
          0,                             // No Accumulation Buffer
          0, 0, 0, 0,                    // Accumulation Bits Ignored
          16,                            // 16Bit Z-Buffer (Depth Buffer)
          0,                             // No Stencil Buffer
          0,                             // No Auxiliary Buffer
          PFD_MAIN_PLANE,                // Main Drawing Layer
          0,                             // Reserved
          0, 0, 0                        // Layer Masks Ignored
      };

  if (!(GDICtx = GetDC(MainWindowHnd)))
    return false;

  if (!(PixelFormat = ChoosePixelFormat(GDICtx, &pfd)))
    return false;

  if (!SetPixelFormat(GDICtx, PixelFormat, &pfd))
    return false;

  if (!(Ctx = wglCreateContext(GDICtx)))
    return false;

  if (!wglMakeCurrent(GDICtx, Ctx))
    return false;

  SetForegroundWindow(MainWindowHnd);
  SetFocus(MainWindowHnd);

  return true;
}
