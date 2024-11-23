#include <Core/Assert/Assert.h>
#include <Render/Private/LogRender.h>
#include <Render/Shaders/Shaders.h>
#include <glad/glad.h>

namespace Render
{
u32 Shader::GetId() const
{
  return Id_;
}
Shader::Kind Shader::GetKind() const
{
  using enum Kind;
  if (!glIsShader(Id_))
    return Invalid;

  GLint param;
  glGetShaderiv(Id_, GL_SHADER_TYPE, &param);

  if (glGetError() != GL_NO_ERROR)
    return Invalid;

  return GLTypeToKind((GLenum)param);
}
Shader::Kind Shader::GLTypeToKind(u32 const GLType)
{
  using enum Kind;
  switch (GLType)
  {
  case GL_COMPUTE_SHADER:
    return ComputeShader;
  case GL_VERTEX_SHADER:
    return VertexShader;
  case GL_TESS_CONTROL_SHADER:
    return TessControlShader;
  case GL_TESS_EVALUATION_SHADER:
    return TessEvaluationShader;
  case GL_GEOMETRY_SHADER:
    return GeometryShader;
  case GL_FRAGMENT_SHADER:
    return FragmentShader;
  }
  checkf(false, "Invalid Shader Type `%d`", GLType);
  __assume(0);
}
u32 Shader::KindToGLType(Kind const Kind)
{
  using enum Kind;
  switch (Kind)
  {
  case ComputeShader:
    return GL_COMPUTE_SHADER;
  case VertexShader:
    return GL_VERTEX_SHADER;
  case TessControlShader:
    return GL_TESS_CONTROL_SHADER;
  case TessEvaluationShader:
    return GL_TESS_EVALUATION_SHADER;
  case GeometryShader:
    return GL_GEOMETRY_SHADER;
  case FragmentShader:
    return GL_FRAGMENT_SHADER;
  }
  checkf(false, "Invalid Kind `%d`", (int)Kind);
  __assume(0);
}
Status Shader::GetStatus() const
{
  using enum Status;
  if (!glIsShader(Id_))
    return Invalid;

  GLint param;

  glGetShaderiv(Id_, GL_DELETE_STATUS, &param);
  if (glGetError() != GL_NO_ERROR)
    return Invalid;

  if (param)
    return MarkedForDeletion;

  glGetShaderiv(Id_, GL_COMPILE_STATUS, &param);
  if (glGetError() != GL_NO_ERROR)
    return Invalid;

  if (param)
    return Compiled;

  return Invalid;
}

Core::String<char> Shader::GetInfoLog() const
{
  if (!glIsShader(Id_))
    return {};

  GLint param;
  glGetShaderiv(Id_, GL_INFO_LOG_LENGTH, &param);
  if (glGetError() != GL_NO_ERROR)
    return {};

  GLint              len{};
  Core::String<char> log(param, '\0');
  glGetShaderInfoLog(Id_, param, &len, log.Data());
  if (glGetError() != GL_NO_ERROR)
    return {};

  log.Resize(len);
  return log;
}
Shader::Shader(Kind Kind, Core::StringView<char> Source)
{
  GLenum const shaderType = KindToGLType(Kind);
  Id_                     = glCreateShader(shaderType);

  GLint const len = Source.Size();
  glShaderSource(Id_, 1, Source.PData(), &len);
  glCompileShader(Id_);

  if (Status::Compiled != GetStatus())
    GE_LOG(LogRender, Core::Verbosity::Error, "Couldn't compile shader.\n%s", GetInfoLog().Data());
}
Shader::Shader(Kind Kind, Core::Vector<Core::StringView<char>> Sources)
{
  Core::Vector<char const*> srcs;
  srcs.Reserve(Sources.Size());
  for (auto const& view : Sources)
    srcs.EmplaceBackUnsafe(view.Data());

  Core::Vector<GLint> lens;
  lens.Reserve(Sources.Size());
  for (auto const& view : Sources)
    lens.EmplaceBackUnsafe(view.Size());

  GLenum const shaderType = KindToGLType(Kind);
  Id_                     = glCreateShader(shaderType);

  glShaderSource(Id_, Sources.Size(), srcs.Data(), &lens[0]);
  glCompileShader(Id_);

  if (GetStatus() == Status::Compiled)
    GE_LOG(LogRender, Core::Verbosity::Error, "Couldn't compile shader.\n%s", GetInfoLog().Data());
}
Shader::~Shader()
{
  glDeleteShader(Id_);
}

Program::Program(Core::Span<Shader> Shaders)
{
  Id_ = glCreateProgram();
  for (auto const& shader : Shaders)
    glAttachShader(Id_, shader.GetId());
  glLinkProgram(Id_);

  if (GetStatus() != Status::Compiled)
}
Program::~Program()
{
  glDeleteProgram(Id_);
}
Status Program::GetStatus() const
{
  using enum Status;
  if (!glIsProgram(Id_))
    return Invalid;

  GLint param;

  glGetProgramiv(Id_, GL_DELETE_STATUS, &param);
  if (glGetError() != GL_NO_ERROR)
    return Invalid;

  if (param)
    return MarkedForDeletion;

  glGetProgramiv(Id_, GL_LINK_STATUS, &param);
  if (glGetError() != GL_NO_ERROR)
    return Invalid;

  if (param)
    return Compiled;

  return Invalid;
}
bool Program::IsActive() const
{
  GLboolean isCurrentProgram;
  glGetBooleanv(GL_CURRENT_PROGRAM, &isCurrentProgram);
  if (glGetError() != GL_NO_ERROR)
    return false;

  return isCurrentProgram;
}
void Program::Activate()
{
  glUseProgram(Id_);
}
Core::String<char> Program::GetInfoLog() const
{
  if (!glIsProgram(Id_))
    return {};

  GLint param;
  glGetProgramiv(Id_, GL_INFO_LOG_LENGTH, &param);
  if (glGetError() != GL_NO_ERROR)
    return {};

  GLint              len{};
  Core::String<char> log(param, '\0');
  glGetProgramInfoLog(Id_, param, &len, log.Data());
  if (glGetError() != GL_NO_ERROR)
    return {};

  log.Resize(len);
  return log;
}
} // namespace Render
