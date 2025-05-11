#include <Core/Platform/Platform.h>
#include <Engine/Components/SpriteComponent.h>
#include <Engine/Components/TransformComponent.h>
#include <Engine/Entities/ActorBase.h>
#include <Engine/Events/ECS/ComponentAttached.h>
#include <Engine/Events/ECS/ComponentDetached.h>
#include <Engine/Events/Renderer/EventResizeWindow.h>
#include <Engine/Interfaces/IEnvironment.h>
#include <Engine/LogRenderer.h>
#include <Engine/SubSystems/Rendering/RenderingSubSystem.h>
#include <Windows.h>
#include <bit>
#include <glad/glad.h>
#include <glad/wgl.h>

GE_DEFINE_TYPE_METADATA(Engine::RenderingSubSystem, Engine::TypeMetaData::EngineSubSystem)

namespace Engine
{
namespace
{
constexpr wchar_t WINDOW_CLASS_NAME[] = L"GE_GAME_ENGINE_RENDERER_SUBSYSTEM_CLASS";

HDC   GDICtx{};
HGLRC OpenGLCtx{};
HWND  MainWindowHnd{};
i32   PixelFormat{};

void APIENTRY OpenGLMessageCallback(GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, GLchar const* Message, void const* UserParam)
{
  (void)UserParam;

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
  if (Core::IsDebuggerAttached() && verbosity == Error)
    Core::DebugBreak();
}
} // namespace

RenderingSubSystem::RenderingSubSystem()
    : Super()
{
  checkf(std::has_single_bit((u32)TileSize_) && TileSize_ > 0, "TileSize must be a power of 2.");
}
void RenderingSubSystem::Tick(f32 DeltaTime)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  using namespace Components;
  for (auto const& [ID, Components] : RenderingComponents_)
  {
    u32 const* programID = Shaders_.Find(ID);
    checkf(programID, "Failed to find shader program ID");
    glUseProgram(*programID);

    if (ID == SpriteComponent::GetStaticTypeMetaData().ID_)
    {
      for (auto const* Component : Components)
      {
        auto const* sprite    = (SpriteComponent*)Component;
        auto const& transform = sprite->Owner()->Transform_;

        float const vertices[12] = {
            transform.Position_.X() - 0.5f,
            transform.Position_.Y() + 0.5f,
            transform.Position_.Z(),
            transform.Position_.X() + 0.5f,
            transform.Position_.Y() + 0.5f,
            transform.Position_.Z(),
            transform.Position_.X() - 0.5f,
            transform.Position_.Y() - 0.5f,
            transform.Position_.Z(),
            transform.Position_.X() + 0.5f,
            transform.Position_.Y() - 0.5f,
            transform.Position_.Z(),
        };

        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
      }
    }
  }

  SwapBuffers(GDICtx);
}
RenderingSubSystem::~RenderingSubSystem()
{
  Cleanup();
}
void RenderingSubSystem::PreInitialize()
{
  Super::PreInitialize();

  using enum IEnvironment::RunningMode;
  if (GlobalEnvironment->GetRunningMode() != Client && GlobalEnvironment->GetRunningMode() != Standalone)
    return; // no rendering done

  struct CleanupHandler
  {
    RenderingSubSystem* subSystem{};
    bool                success         = false;
    bool                hasLoadedOpenGL = false;
    ~CleanupHandler()
    {
      checkf(success, "Failed to initialize RenderingSubSystem.");
      if (!success)
      {
        GLenum const       glError = hasLoadedOpenGL ? glGetError() : 0;
        Core::String<char> err;
        GetLastErrorString(err);
        GE_LOG(LogRenderer, Core::Verbosity::Error, "Error while initializing OpenGL: %d - %.*s.", glError, err.Size(), err.Data());
        subSystem->Cleanup();
      }
    }
  } cleanupHandler{.subSystem = this};

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
      32,
      0, 0, 0, 0, 0, 0,
      0,
      0,
      0,
      0, 0, 0, 0,
      24,
      8,
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

  if (!(OpenGLCtx = wglCreateContext(GDICtx)))
    return;

  if (!wglMakeCurrent(GDICtx, OpenGLCtx))
    return;

  if (!gladLoadGL() || !gladLoadWGL(GDICtx))
    return;

  cleanupHandler.hasLoadedOpenGL = true;

  int const attributes[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
      WGL_CONTEXT_MINOR_VERSION_ARB, 6,
      WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifndef GE_BUILD_CONFIG_RELEASE
      WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
      0};
  HGLRC newOpenGLCtx = wglCreateContextAttribsARB(GDICtx, NULL, attributes);
  if (!newOpenGLCtx)
    return;

  if (!wglMakeCurrent(GDICtx, newOpenGLCtx))
  {
    wglDeleteContext(newOpenGLCtx);
    return;
  }

  auto oldOpenGLCtx = OpenGLCtx;
  OpenGLCtx         = newOpenGLCtx;

  wglDeleteContext(oldOpenGLCtx);

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
  Super::PostInitialize();

  using enum IEnvironment::RunningMode;
  if (GlobalEnvironment->GetRunningMode() == Client || GlobalEnvironment->GetRunningMode() == Standalone)
  {
    checkf(OpenGLCtx, "Failed to initialize RenderingSubSystem.");
    CompileShaders();
  }
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

    if (auto* Components = RenderingComponents_.Find(ID))
    {
      Components->EmplaceBack(attachedEvent->Component_);
    }
    else
    {
      auto* components = RenderingComponents_.TryEmplace(ID);
      checkf(components, "Failed to register component %llu.", ID);
      if (components)
        components->EmplaceBack(attachedEvent->Component_);
    }
    return true;
  }

  if (auto const* detachedEvent = Event.GetAs<EventComponentDetached>())
  {
    u64 const ID = detachedEvent->Component_->GetTypeMetaData().ID_;
    if (!IsComponentHandledByUs(ID))
      return false;

    auto* components = RenderingComponents_.Find(ID);
    checkf(components, "Failed to find component %llu.", ID);
    if (components)
      components->EraseIf([C = detachedEvent->Component_](Components::ComponentBase* const& Registered) { return Registered == C; });
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

  for (u32 programID : Shaders_.Values())
    glDeleteProgram(programID);

  if (OpenGLCtx)
  {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(OpenGLCtx);
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
void RenderingSubSystem::CompileShaders()
{
  { // SpriteComponent
    Core::StringView<char> const vsSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main()
    {
      gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
    )";
    GLuint const                 vsID     = glCreateShader(GL_VERTEX_SHADER);
    GLint const                  vsLen    = vsSource.Size();
    glShaderSource(vsID, 1, vsSource.PData(), &vsLen);
    glCompileShader(vsID);

    Core::StringView<char> const fsSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main()
    {
      FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    )";
    GLuint const                 fsID     = glCreateShader(GL_FRAGMENT_SHADER);
    GLint const                  fsLen    = fsSource.Size();
    glShaderSource(fsID, 1, fsSource.PData(), &fsLen);
    glCompileShader(fsID);

    u32 const programID = glCreateProgram();
    glAttachShader(programID, vsID);
    glAttachShader(programID, fsID);
    glLinkProgram(programID);

    glDeleteShader(vsID);
    glDeleteShader(fsID);

    u64 const typeID = Components::SpriteComponent::GetStaticTypeMetaData().ID_;
    Shaders_.TryEmplace(typeID, programID);
  }
}
} // namespace Engine