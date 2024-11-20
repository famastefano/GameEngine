#pragma once

#include <Core/Definitions.h>

namespace Math
{
struct Vec3D
{
  f32 X_, Y_, Z_;

  constexpr Vec3D();
  constexpr Vec3D(f32 X, f32 Y, f32 Z);

  constexpr Vec3D(Vec3D const&)            = default;
  constexpr Vec3D& operator=(Vec3D const&) = default;

  Vec3D& Normalize();
  Vec3D  Normalized() const;

  f32           Length() const;
  constexpr f32 LengthSquared() const;

  f32 CosAngleBetween(Vec3D const& Other) const;

  constexpr f32   Dot(Vec3D const& Other) const;
  constexpr Vec3D Cross(Vec3D const& Other) const;

  constexpr friend Vec3D operator+(Vec3D const& v, Vec3D const& u);
  constexpr friend Vec3D operator-(Vec3D const& v, Vec3D const& u);
  constexpr friend f32   operator*(Vec3D const& v, Vec3D const& u);
  constexpr friend Vec3D operator*(Vec3D const& v, f32 const Scalar);
};
} // namespace Math
