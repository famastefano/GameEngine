#include <Core/Container/FlatMap.h>
#include <Core/Hash/Hash.h>
#include <Render/RenderingSystem.h>
#include <Render/Shaders/Shaders.h>
#include <Windows.h>
#include <glad/glad.h>
#include <memory>

namespace Render
{
static HDC   GDICtx{};
static HGLRC Ctx{};
static HWND  MainWindowHnd{};
static i32   PixelFormat{};

static Core::CompactFlatMap<u64, std::shared_ptr<Program>> Programs;
static Core::CompactFlatMap<u64, std::shared_ptr<Shader>>  Shaders;

bool RenderingSystem::Init(void* RenderingWindowHandle, bool Fullscreen)
{
  MainWindowHnd = (HWND)RenderingWindowHandle;

  PIXELFORMATDESCRIPTOR pfd = {
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
    return false;

  if (!(PixelFormat = ChoosePixelFormat(GDICtx, &pfd)))
    return false;

  if (!SetPixelFormat(GDICtx, PixelFormat, &pfd))
    return false;

  if (!(Ctx = wglCreateContext(GDICtx)))
    return false;

  if (!wglMakeCurrent(GDICtx, Ctx))
    return false;

  if (!gladLoadGL())
    return false;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  return true;
}
void RenderingSystem::Shutdown()
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
}
void RenderingSystem::ResizeViewport(i32 const Width, i32 const Height)
{
  glViewport(0, 0, Width, Height);
}
void RenderingSystem::Render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //  glBindVertexArray(VAO);
  //  glDrawArrays(GL_TRIANGLES, 0, 3);
  SwapBuffers(GDICtx);
}
std::shared_ptr<Shader> RenderingSystem::FindShader(Core::StringView<char> Name)
{
  if (auto* shader = Shaders.Find(Name.CalculateHash()))
    return *shader;
  return nullptr;
}
std::shared_ptr<Shader> RenderingSystem::CreateShader(Core::StringView<char> Name, Shader::Kind Kind, Core::StringView<char> Source)
{
  auto const hash = Name.CalculateHash();
  if (auto* shader = Shaders.Find(hash))
    return *shader;

  if (Shaders.TryEmplace(hash, std::shared_ptr<Shader>(new Shader(Kind, Source))))
    return Shaders[hash];

  checkf(false, "Failed to create shader `%.*s`", Name.Size(), Name.Data());
}
std::shared_ptr<Shader> RenderingSystem::CreateShader(Core::StringView<char> Name, Shader::Kind Kind, Core::Span<Core::StringView<char>> Sources)
{
  auto const hash = Name.CalculateHash();
  if (auto* shader = Shaders.Find(hash))
    return *shader;

  if (Shaders.TryEmplace(hash, std::shared_ptr<Shader>(new Shader(Kind, Sources))))
    return Shaders[hash];

  checkf(false, "Failed to create shader `%.*s`", Name.Size(), Name.Data());
}
std::shared_ptr<Program> RenderingSystem::FindProgram(Core::StringView<char> Name)
{
  if (auto* program = Programs.Find(Name.CalculateHash()))
    return *program;
  return nullptr;
}
std::shared_ptr<Program> RenderingSystem::CreateProgram(Core::StringView<char> Name, std::initializer_list<Shader const*> Shaders)
{
  auto const hash = Name.CalculateHash();
  if (auto* program = Programs.Find(hash))
    return *program;

  if (Programs.TryEmplace(hash, std::shared_ptr<Program>(new Program(Shaders))))
    return Programs[hash];

  checkf(false, "Failed to create shader `%.*s`", Name.Size(), Name.Data());
}
void RenderingSystem::DestroyShader(Core::StringView<char> Name)
{
  Shaders.TryRemove(Name.CalculateHash());
}
void RenderingSystem::DestroyProgram(Core::StringView<char> Name)
{
  Programs.TryRemove(Name.CalculateHash());
}
} // namespace Render
