#include <Math/Vector.h>
#include <cmath>

namespace Math
{
constexpr Vec3D::Vec3D()
    : Vec3D(0, 0, 0)
{
}
constexpr Vec3D::Vec3D(f32 X, f32 Y, f32 Z)
    : X_(X)
    , Y_(Y)
    , Z_(Z)
{
}
Vec3D& Vec3D::Normalize()
{
  f32 const length  = Length();
  X_               /= length;
  Y_               /= length;
  Z_               /= length;
  return *this;
}
Vec3D Vec3D::Normalized() const
{
  return Vec3D(*this).Normalize();
}
f32 Vec3D::Length() const
{
  return sqrtf(LengthSquared());
}
constexpr f32 Vec3D::LengthSquared() const
{
  return *this * *this;
}
f32 Vec3D::CosAngleBetween(Vec3D const& Other) const
{
  return Normalized() * Other.Normalized();
}
constexpr f32 Vec3D::Dot(Vec3D const& Other) const
{
  return Vec3D(*this) * Other;
}
constexpr Vec3D Vec3D::Cross(Vec3D const& Other) const
{
  return {Y_ * Other.Z_ - Z_ * Other.Y_, Z_ * Other.X_ - X_ * Other.Z_, X_ * Other.Y_ - Y_ * Other.X_};
}
constexpr Vec3D operator+(Vec3D const& v, Vec3D const& u)
{
  return {v.X_ + u.X_, v.Y_ + u.Y_, v.Z_ + u.Z_};
}
constexpr Vec3D operator-(Vec3D const& v, Vec3D const& u)
{
  return {v.X_ - u.X_, v.Y_ - u.Y_, v.Z_ - u.Z_};
}
constexpr f32 operator*(Vec3D const& v, Vec3D const& u)
{
  return v.X_ * u.X_ + v.Y_ * u.Y_ + v.Z_ + u.Z_;
}
constexpr Vec3D operator*(Vec3D const& v, f32 const Scalar)
{
  return {v.X_ * Scalar, v.Y_ * Scalar, v.Z_ * Scalar};
}
} // namespace Math
