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
AABB::AABB(Math::Vec2Di Center, i32 Size)
    : AABB(Center, {Size / 2, Size / 2})
{
}
AABB::AABB(Math::Vec2Di Center, Math::Vec2Di Extents)
    : Center_(Center)
    , Extents_(Extents)
{
}
bool AABB::Overlaps(AABB Other, bool OnlyIntersection) const
{
  Math::Vec2Di const tl{Center_.X() - Extents_.X(), Center_.Y() - Extents_.Y()};
  Math::Vec2Di const br{Center_.X() + Extents_.X(), Center_.Y() + Extents_.Y()};

  Math::Vec2Di const otherTL{Other.Center_.X() - Other.Extents_.X(), Other.Center_.Y() - Other.Extents_.Y()};
  Math::Vec2Di const otherBR{Other.Center_.X() + Other.Extents_.X(), Other.Center_.Y() + Other.Extents_.Y()};

  i32 const leftX   = std::max(tl.X(), otherTL.X());
  i32 const rightX  = std::min(br.X(), otherBR.X());
  i32 const topY    = std::max(tl.Y(), otherTL.Y());
  i32 const bottomY = std::min(br.Y(), otherBR.Y());

  bool const overlaps = leftX < rightX && topY < bottomY;
  if (!OnlyIntersection || !overlaps)
    return overlaps;

  Math::Vec2Di const otherVertices[4] = {
      {Other.Center_.X() - Other.Extents_.X(), Other.Center_.Y() - Other.Extents_.Y()},
      {Other.Center_.X() + Other.Extents_.X(), Other.Center_.Y() - Other.Extents_.Y()},
      {Other.Center_.X() + Other.Extents_.X(), Other.Center_.Y() + Other.Extents_.Y()},
      {Other.Center_.X() - Other.Extents_.X(), Other.Center_.Y() + Other.Extents_.Y()},
  };
  bool contained  = Contains(otherVertices[0]);
  contained      &= Contains(otherVertices[1]);
  contained      &= Contains(otherVertices[2]);
  contained      &= Contains(otherVertices[3]);
  return !contained;
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

  Math::Vec2Di center  = (max + min);
  center.X()          /= 2;
  center.Y()          /= 2;

  Math::Vec2Di extents  = max - min;
  extents.X()          /= 2;
  extents.Y()          /= 2;

  return AABB(center, extents);
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

  Math::Vec2Di center  = (max + min);
  center.X()          /= 2;
  center.Y()          /= 2;

  Math::Vec2Di extents  = max - min;
  extents.X()          /= 2;
  extents.Y()          /= 2;

  return AABB(center, extents);
}
} // namespace Physics
