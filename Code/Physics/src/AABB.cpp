#include <Core/Assert/Assert.h>
#include <Physics/Collisions/AABB.h>
#include <algorithm>
#include <cmath>

namespace Physics
{
AABB::AABB()
    : AABB({0, 0}, 0)
{
}
AABB::AABB(Math::Vec2Di const Center, i32 const Extent)
    : AABB(Center, {Extent, Extent})
{
}
AABB::AABB(Math::Vec2Di const Center, Math::Vec2Di const Extents)
    : Center_(Center)
    , Extents_(Extents)
{
  checkf(Extents.X() * Extents.Y() >= 0, "Degenerate AABB.");
}
bool AABB::Contains(AABB const Other) const
{
  /*
   * Legend:
   * C = center, d = half-diagonal, h = half-height, w = half-width
   * ___________
   * |\        |
   * | \       |
   * |  \d     |
   * |   \   w |
   * |    C----|
   * |    |    |
   * |    |h   |
   * |    |    |
   * |____|____|
   */

  // We use squared lengths to avoid sqrt
  using Math::Vec2Di;

  Vec2Di const C0       = Center_;
  Vec2Di const C1       = Other.Center_;
  i32 const    d0       = (Vec2Di{C0.X() + Extents_.X(), C0.Y() + Extents_.Y()} - Vec2Di{C0}).LengthSquared();
  i32 const    d1       = (Vec2Di{C1.X() + Other.Extents_.X(), C1.Y() + Other.Extents_.Y()} - Vec2Di{C1}).LengthSquared();
  Vec2Di const distC0C1 = C0 - C1;

  // We are either touching on 1 vertex or even far apart
  if (distC0C1.LengthSquared() >= d0 + d1)
    return false;

  i32 const w0 = Extents_.X() * Extents_.X();
  i32 const w1 = Other.Extents_.X() * Other.Extents_.X();
  i32 const h0 = Extents_.Y() * Extents_.Y();
  i32 const h1 = Other.Extents_.Y() * Other.Extents_.Y();

  i32 const  maxH      = std::max(h0, h1);
  i32 const  minH      = std::min(h0, h1);
  i32 const  maxW      = std::max(w0, w1);
  i32 const  minW      = std::min(w0, w1);
  bool const contained = abs(distC0C1.X()) <= abs(maxW - minW)
                      && abs(distC0C1.Y()) <= abs(maxH - minH)
                      // Edge-case where C0 == C1
                      && (w0 <= w1 && h0 <= h1 || w1 <= w0 && h1 <= h0);
  return contained;
}
bool AABB::Contains(Math::Vec2Di const Point) const
{
  auto const p = Point - Center_;
  return abs(p.X()) <= Extents_.X() && abs(p.Y()) <= Extents_.Y();
}
AABB AABB::CalculateAABB(Core::Span<Math::Vec2Di const> Points)
{
  if (Points.IsEmpty())
    return {};

  Math::Vec2Di min = Points[0], max = Points[0];
  for (auto const& p : Points)
  {
    min.X() = std::min(min.X(), p.X());
    min.Y() = std::min(min.Y(), p.Y());
    max.X() = std::max(max.X(), p.X());
    max.Y() = std::max(max.Y(), p.Y());
  }

  Math::Vec2Di center  = max + min;
  center.X()          /= 2;
  center.Y()          /= 2;

  Math::Vec2Di extents  = max - min;
  extents.X()          /= 2;
  extents.Y()          /= 2;

  return {center, extents};
}
AABB AABB::CalculateAABB(Core::Span<const AABB> AABBs)
{
  if (AABBs.IsEmpty())
    return {};

  Math::Vec2Di min, max;
  min.X() = AABBs[0].Center_.X() - AABBs[0].Extents_.X();
  min.Y() = AABBs[0].Center_.Y() - AABBs[0].Extents_.Y();
  max.X() = AABBs[0].Center_.X() + AABBs[0].Extents_.X();
  max.Y() = AABBs[0].Center_.Y() + AABBs[0].Extents_.Y();

  for (auto const& box : AABBs)
  {
    min.X() = std::min(min.X(), box.Center_.X() - box.Extents_.X());
    min.Y() = std::min(min.Y(), box.Center_.Y() - box.Extents_.Y());
    max.X() = std::max(max.X(), box.Center_.X() + box.Extents_.X());
    max.Y() = std::max(max.Y(), box.Center_.Y() + box.Extents_.Y());
  }

  Math::Vec2Di center  = max + min;
  center.X()          /= 2;
  center.Y()          /= 2;

  Math::Vec2Di extents  = max - min;
  extents.X()          /= 2;
  extents.Y()          /= 2;

  return {center, extents};
}
i32 AABB::Area() const
{
  /*
   *  Extents_.X() = a, Extents_.Y() = b
   *  w = 2a, h = 2b
   *  Area = (w*h)/2 = (2a*2b)/2 = 4ab/2 = 2ab
   */
  return Extents_.X() * Extents_.Y() * 2;
}
} // namespace Physics
