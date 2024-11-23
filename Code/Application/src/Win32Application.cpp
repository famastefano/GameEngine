#include <Application/LogApplication.h>
#include <Core/Assert/Assert.h>
#include <Input/Base/Translator.h>
#include <Render/RenderingSystem.h>
#include <Render/Shaders/Shaders.h>
#include <Windows.h>
#include <hidusage.h>

wchar_t const CLASS_NAME[]           = L"GE_MAIN_WINDOW_CLASS";
char const    VERTEX_SHADER_NAME[]   = "BasicVertexShader";
char const    FRAGMENT_SHADER_NAME[] = "BasicFragmentShader";
char const    PROGRAM_NAME[]         = "BasicProgram";
HINSTANCE     hInstance{};
HWND          MainWindowHnd{};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool             CreateRenderingWindow(wchar_t const* Title, int const Width, int const Height);
bool             InitRawInput();

struct EngineShutdownOnExit
{
  ~EngineShutdownOnExit()
  {
    Render::RenderingSystem::Shutdown();

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

  if (!verifyf(Render::RenderingSystem::Init(MainWindowHnd, false), "Couldn't initialize Rendering System."))
    return -2;

  if (!verifyf(InitRawInput(), "Couldn't initialize raw input."))
    return -3;

  ShowWindow(MainWindowHnd, nShowCmd);

  {

    auto vertexShader = Render::RenderingSystem::CreateShader(VERTEX_SHADER_NAME, Render::Shader::Kind::VertexShader, R"(
      #version 330 core
      layout (location = 0) in vec3 aPos;
      void main()
      {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
      }
      )");

    auto fragmentShader = Render::RenderingSystem::CreateShader(FRAGMENT_SHADER_NAME, Render::Shader::Kind::FragmentShader, R"(
      #version 330 core
      out vec4 FragColor;
      void main()
      {
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
      }
      )");

    Render::RenderingSystem::CreateProgram(PROGRAM_NAME, {vertexShader.get(), fragmentShader.get()});

    // constexpr float const vertices[] = {
    //     -0.5f, -0.5f, 0.0f,
    //     0.5f, -0.5f, 0.0f,
    //     0.0f, 0.5f, 0.0f};
    //
    // u32 VBO;
    // glGenBuffers(1, &VBO);
    // glGenVertexArrays(1, &VAO);
    //
    //// 0. copy our vertices array in a buffer for OpenGL to use
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //// 1. then set the vertex attributes pointers
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    // glEnableVertexAttribArray(0);
  }

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

    auto program = Render::RenderingSystem::FindProgram(PROGRAM_NAME);
    if (!program->IsActive())
      program->Activate();

    Render::RenderingSystem::Render();
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
    Render::RenderingSystem::ResizeViewport(LOWORD(lParam), HIWORD(lParam));
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
