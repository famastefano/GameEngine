#pragma once

#include <Core/Container/String.h>
#include <Core/Definitions.h>
#include <Core/Helpers.h>
#include <Render/API.h>

namespace Render
{
enum class Status
{
  Invalid,
  MarkedForDeletion,
  Compiled,
};

class RENDER_API Shader
{
  u32 Id_;

public:
  u32 GetId() const;

  enum class Kind
  {
    Invalid,
    ComputeShader,
    VertexShader,
    TessControlShader,
    TessEvaluationShader,
    GeometryShader,
    FragmentShader,
  };

  Kind GetKind() const;

  static Kind GLTypeToKind(u32 const GLType);
  static u32  KindToGLType(Kind const Kind);

  Status GetStatus() const;

  Core::String<char> GetInfoLog() const;

  Shader(Kind Kind, Core::StringView<char> Source);
  Shader(Kind Kind, Core::Vector<Core::StringView<char>> Sources);
  virtual ~Shader();
};

class RENDER_API Program
{
  u32 Id_;

public:
  Program(Core::Span<Shader> Shaders);
  virtual ~Program();

  Status GetStatus() const;

  bool IsActive() const;
  void Activate();

  Core::String<char> GetInfoLog() const;
};
} // namespace Render
