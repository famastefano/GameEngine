#pragma once

#include <Core/Container/String.h>
#include <Core/Definitions.h>
#include <Core/Helpers.h>
#include <Render/API.h>

namespace Render
{
class RenderingSystem;

enum class Status
{
  Invalid,
  MarkedForDeletion,
  Compiled,
};

class RENDER_API Shader
{
  friend class RenderingSystem;

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

protected:
  Shader(Kind Kind, Core::StringView<char> Source);
  Shader(Kind Kind, Core::Span<Core::StringView<char>> Sources);

public:
  virtual ~Shader();
};

class RENDER_API Program
{
  friend class RenderingSystem;
  u32 Id_;

protected:
  Program(Core::Span<Shader const*> Shaders);

public:
  virtual ~Program();

  Status GetStatus() const;

  bool IsActive() const;
  void Activate();

  Core::String<char> GetInfoLog() const;
};
} // namespace Render
