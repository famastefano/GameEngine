#pragma once

#include <Core/Definitions.h>
#include <Render/API.h>
#include <Render/Shaders/Shaders.h>

namespace Render
{
class RENDER_API RenderingSystem
{
public:
  static bool Init(void* RenderingWindowHandle, bool Fullscreen);
  static void Shutdown();
  static void ResizeViewport(i32 const Width, i32 const Height);
  static void Render();

  static std::shared_ptr<Shader> FindShader(Core::StringView<char> Name);
  static std::shared_ptr<Shader> CreateShader(Core::StringView<char> Name, Shader::Kind Kind, Core::StringView<char> Source);
  static std::shared_ptr<Shader> CreateShader(Core::StringView<char> Name, Shader::Kind Kind, Core::Span<Core::StringView<char>> Sources);

  static std::shared_ptr<Program> FindProgram(Core::StringView<char> Name);
  static std::shared_ptr<Program> CreateProgram(Core::StringView<char> Name, std::initializer_list<Shader const*> Shaders);

  static void DestroyShader(Core::StringView<char> Name);
  static void DestroyProgram(Core::StringView<char> Name);
};
} // namespace Render
