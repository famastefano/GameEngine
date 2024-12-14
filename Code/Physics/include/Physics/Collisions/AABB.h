#pragma once

#include <Core/Container/Span.h>
#include <Core/Definitions.h>
#include <Core/Helpers.h>
#include <Math/Vector.h>
#include <Physics/API.h>

namespace Physics
{
class PHYSICS_API AABB
{
public:
  Math::Vec2Di Center_;
  Math::Vec2Di Extents_;

  AABB();
  AABB(Math::Vec2Di Center, i32 Size);
  AABB(Math::Vec2Di Center, Math::Vec2Di Extents);

  [[nodiscard]] bool Contains(AABB Other) const;
  [[nodiscard]] bool Contains(Math::Vec2Di Point) const;

  [[nodiscard]] i32 Area() const;

  static AABB CalculateAABB(Core::Span<Math::Vec2Di const> Points);
  static AABB CalculateAABB(Core::Span<const AABB> AABBs);
};
} // namespace Physics
