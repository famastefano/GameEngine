#include <Render/Renderable/Renderable.h>
#include <glad/glad.h>

namespace Render
{
Renderable::Renderable(Kind Kind, Core::Span<f32> Vertices)
    : Kind_(Kind)
{
  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);
  glGenBuffers(1, &VBO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  glBufferData(GL_ARRAY_BUFFER, Vertices.AllocSize(), Vertices.Data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertices.AllocSize(), nullptr);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}
Renderable::Renderable(Kind Kind, Core::Span<f32> Vertices, Core::Span<i32> Indices)
{
}
Renderable::~Renderable()
{
}
} // namespace Render
