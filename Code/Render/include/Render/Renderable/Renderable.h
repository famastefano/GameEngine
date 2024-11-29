#pragma once

#include <Core/Container/Span.h>
#include <Core/Definitions.h>
#include <Render/API.h>

namespace Render
{
class RenderingSystem;

class RENDER_API Renderable
{
public:
  enum class Kind : u8
  {
    LineStrip,
    LineLoop,
    Lines,
    TriangleStrip,
    TriangleFan,
    Triangles,
  };

protected:
  u32  VAO_{};
  u32  VBO_{};
  u32  EBO_{};
  Kind Kind_;

  Renderable(Kind Kind, Core::Span<f32> Vertices);
  Renderable(Kind Kind, Core::Span<f32> Vertices, Core::Span<i32> Indices);

public:
  virtual ~Renderable();
};
} // namespace Render
