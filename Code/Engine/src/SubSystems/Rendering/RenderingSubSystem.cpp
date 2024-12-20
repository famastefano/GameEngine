#include <Engine/Components/ComponentBase.h>
#include <Engine/Components/SpriteComponent.h>
#include <Engine/Components/TransformComponent.h>
#include <Engine/Entities/ActorBase.h>
#include <Engine/Events/ECS/ComponentAttached.h>
#include <Engine/Events/ECS/ComponentDetached.h>
#include <Engine/Events/Renderer/EventResizeWindow.h>
#include <Engine/Interfaces/IEnvironment.h>
#include <Engine/LogRenderer.h>
#include <Engine/SubSystems/Globals.h>
#include <Engine/SubSystems/Rendering/RenderingSubSystem.h>
#include <Engine/SubSystems/SubSystemRegistration.h>
#include <Windows.h>
#include <bit>
#include <glad/glad.h>

namespace Engine
{
GE_REGISTER_SUBSYSTEM(SubSystemType::Engine, RenderingSubSystem)

RenderingSubSystem* CurrentRenderingSubSystem{};

static Core::FlatMap<u64, Core::Vector<Components::ComponentBase*>> RenderingComponents;

namespace
{
constexpr wchar_t WINDOW_CLASS_NAME[] = L"GE_GAME_ENGINE_RENDERER_SUBSYSTEM_CLASS";

HDC   GDICtx{};
HGLRC Ctx{};
HWND  MainWindowHnd{};
i32   PixelFormat{};

void APIENTRY OpenGLMessageCallback(GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, GLchar const* Message, void const* UserParam)
{
  auto const* source = [Source] {
    switch (Source)
    {
    case GL_DEBUG_SOURCE_API:
      return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      return "WINDOW SYSTEM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return "SHADER COMPILER";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      return "THIRD PARTY";
    case GL_DEBUG_SOURCE_APPLICATION:
      return "APPLICATION";
    case GL_DEBUG_SOURCE_OTHER:
      return "OTHER";
    }
    return "UNKNOWN";
  }();

  auto const* type = [Type] {
    switch (Type)
    {
    case GL_DEBUG_TYPE_ERROR:
      return "ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      return "DEPRECATED_BEHAVIOR";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return "UNDEFINED_BEHAVIOR";
    case GL_DEBUG_TYPE_PORTABILITY:
      return "PORTABILITY";
    case GL_DEBUG_TYPE_PERFORMANCE:
      return "PERFORMANCE";
    case GL_DEBUG_TYPE_MARKER:
      return "MARKER";
    case GL_DEBUG_TYPE_OTHER:
      return "OTHER";
    }
    return "UNKNOWN";
  }();

  using enum Core::Verbosity;
  auto verbosity = Info;
  if (Type == GL_DEBUG_TYPE_ERROR || Type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR || Severity == GL_DEBUG_SEVERITY_HIGH)
    verbosity = Error;
  else if (Type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR || Type == GL_DEBUG_TYPE_PERFORMANCE || Severity == GL_DEBUG_SEVERITY_MEDIUM)
    verbosity = Warning;
  GE_LOG(LogRenderer, verbosity, "[%s][%s]#%u: %.*s", source, type, ID, Length, Message);
}
} // namespace

RenderingSubSystem::RenderingSubSystem()
    : Super()
{
  CurrentRenderingSubSystem = this;
  checkf(std::has_single_bit((u32)TileSize_) && TileSize_ > 0, "TileSize must be a power of 2.");
}
void RenderingSubSystem::Tick(f32 DeltaTime)
{
  using namespace Components;
  for (auto const& [ID, Components] : RenderingComponents)
  {
    if (ID == SpriteComponent::GetStaticTypeMetaData().ID_)
    {
      for (auto const* Component : Components)
      {
        auto const* sprite    = (SpriteComponent*)Component;
        auto const* transform = sprite->Owner()->FindComponentChecked<TransformComponent>();
      }
    }
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SwapBuffers(GDICtx);
}
RenderingSubSystem::~RenderingSubSystem()
{
  Cleanup();
}
void RenderingSubSystem::PreInitialize()
{
  EngineSubSystem::PreInitialize();

  using enum IEnvironment::RunningMode;
  if (GlobalEnvironment->GetRunningMode() != Client && GlobalEnvironment->GetRunningMode() != Standalone)
    return; // no rendering done

  struct CleanupHandler
  {
    bool success = false;
    ~CleanupHandler()
    {
      checkf(success, "Failed to initialize RenderingSubSystem.");
      if (!success)
        Cleanup();
    }
  } cleanupHandler;

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

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(OpenGLMessageCallback, nullptr);

  ShowWindow(MainWindowHnd, SW_SHOW);

  cleanupHandler.success = true;
}
void RenderingSubSystem::PostInitialize()
{
  EngineSubSystem::PostInitialize();

  using enum IEnvironment::RunningMode;
  if (GlobalEnvironment->GetRunningMode() == Client || GlobalEnvironment->GetRunningMode() == Standalone)
    checkf(Ctx, "Failed to initialize RenderingSubSystem.");
}
bool RenderingSubSystem::HandleEvent(EventBase& Event)
{
  if (auto const* resizeEvent = Event.GetAs<EventResizeWindow>())
  {
    ResizeViewport(resizeEvent->Width_, resizeEvent->Height_);
    return true;
  }

  if (auto const* attachedEvent = Event.GetAs<EventComponentAttached>())
  {
    u64 const ID = attachedEvent->Component_->GetTypeMetaData().ID_;
    if (!IsComponentHandledByUs(ID))
      return false;

    if (auto* Components = RenderingComponents.Find(ID))
    {
      Components->EmplaceBack(attachedEvent->Component_);
    }
    else
    {
      bool const success = RenderingComponents.TryEmplace(ID);
      checkf(success, "Failed to register component %llu.", ID);
      if (success)
        RenderingComponents[ID].EmplaceBack(attachedEvent->Component_);
    }
    return true;
  }

  if (auto const* detachedEvent = Event.GetAs<EventComponentDetached>())
  {
    u64 const ID = detachedEvent->Component_->GetTypeMetaData().ID_;
    if (!IsComponentHandledByUs(ID))
      return false;

    auto* Components = RenderingComponents.Find(ID);
    checkf(Components, "Failed to find component %llu.", ID);
    if (Components)
      Components->EraseIf([C = detachedEvent->Component_](Components::ComponentBase* const& Registered) { return Registered == C; });
    return true;
  }

  return false;
}
void RenderingSubSystem::ResizeViewport(i32 const Width, i32 const Height)
{
  glViewport(0, 0, Width, Height);
}
void RenderingSubSystem::Cleanup()
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
bool RenderingSubSystem::IsComponentHandledByUs(u64 const ID)
{
  using namespace Components;
  for (u64 const SupportedID : {
           SpriteComponent::GetStaticTypeMetaData().ID_,
       })
  {
    if (ID == SupportedID)
      return true;
  }
  return false;
}
} // namespace Engine
