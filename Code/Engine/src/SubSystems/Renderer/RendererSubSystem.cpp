#include <Engine/Events/Renderer/EventResizeWindow.h>
#include <Engine/Interfaces/IEnvironment.h>
#include <Engine/SubSystems/Renderer/RendererSubSystem.h>
#include <Engine/SubSystems/SubSystemRegistration.h>
#include <Windows.h>
#include <bit>
#include <glad/glad.h>

namespace Engine
{
GE_REGISTER_SUBSYSTEM(SubSystemType::Engine, RendererSubSystem)

namespace
{
constexpr wchar_t WINDOW_CLASS_NAME[] = L"GE_GAME_ENGINE_RENDERER_SUBSYSTEM_CLASS";

HDC   GDICtx{};
HGLRC Ctx{};
HWND  MainWindowHnd{};
i32   PixelFormat{};
} // namespace

RendererSubSystem::RendererSubSystem()
    : Super()
{
  checkf(std::has_single_bit((u32)TileSize_) && TileSize_ > 0, "TileSize must be a power of 2.");
}
void RendererSubSystem::Tick(f32 DeltaTime)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SwapBuffers(GDICtx);
}
RendererSubSystem::~RendererSubSystem()
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

  UnregisterClassW(WINDOW_CLASS_NAME, GetModuleHandle(NULL));
}
void RendererSubSystem::PreInitialize()
{
  EngineSubSystem::PreInitialize();

  DWORD const dwExStyle  = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
  DWORD const dwStyle    = WS_OVERLAPPEDWINDOW;
  RECT const  WindowRect = {
       .left   = 0,
       .top    = 0,
       .right  = 640,
       .bottom = 480,
  };
  WNDCLASS const wc{
      .style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
      .lpfnWndProc   = (WNDPROC)GlobalEnvironment->NativeLoopCallback_,
      .hInstance     = GetModuleHandle(NULL),
      .hIcon         = LoadIcon(NULL, IDI_WINLOGO),
      .hCursor       = LoadCursor(NULL, IDC_ARROW),
      .lpszClassName = WINDOW_CLASS_NAME,
  };

  if (!RegisterClass(&wc))
    return;

  MainWindowHnd = CreateWindowEx(dwExStyle,
                                 WINDOW_CLASS_NAME,
                                 L"Window Title",
                                 dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                 0, 0,
                                 WindowRect.right - WindowRect.left,
                                 WindowRect.bottom - WindowRect.top,
                                 NULL,
                                 NULL,
                                 wc.hInstance,
                                 NULL);
  if (!MainWindowHnd)
    return;

  SetForegroundWindow(MainWindowHnd);
  SetFocus(MainWindowHnd);

  PIXELFORMATDESCRIPTOR const pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
      PFD_TYPE_RGBA,
      16,
      0, 0, 0, 0, 0, 0,
      0,
      0,
      0,
      0, 0, 0, 0,
      16,
      0,
      0,
      PFD_MAIN_PLANE,
      0,
      0, 0, 0};

  if (!(GDICtx = GetDC(MainWindowHnd)))
    return;

  if (!(PixelFormat = ChoosePixelFormat(GDICtx, &pfd)))
    return;

  if (!SetPixelFormat(GDICtx, PixelFormat, &pfd))
    return;

  if (!(Ctx = wglCreateContext(GDICtx)))
    return;

  if (!wglMakeCurrent(GDICtx, Ctx))
    return;

  if (!gladLoadGL())
    return;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  ShowWindow(MainWindowHnd, SW_SHOW);
}
bool RendererSubSystem::HandleEvent(EventBase& Event)
{
  if (Event.GetID() == EventResizeWindow::GetUniqueID())
  {
    auto const& ev = (EventResizeWindow&)Event;
    ResizeViewport(ev.Width_, ev.Height_);
    return true;
  }
  return false;
}
void RendererSubSystem::ResizeViewport(i32 const Width, i32 const Height)
{
  glViewport(0, 0, Width, Height);
}
} // namespace Engine
