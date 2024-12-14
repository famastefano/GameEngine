#pragma once

#include <Core/Definitions.h>
#include <cmath>
#include <type_traits>

namespace Math
{
template <typename T, i32 Dim>
struct Vec
{
  static_assert(Dim > 0);
  static_assert(std::is_arithmetic_v<T>);

  T Elements_[Dim] = {};

  constexpr Vec()
  {
  }
  constexpr Vec(T X)
  {
    Elements_[0] = X;
  }
  constexpr Vec(T X, T Y)
      : Vec(X)
  {
    static_assert(Dim >= 2);
    Elements_[1] = Y;
  }
  constexpr Vec(T X, T Y, T Z)
      : Vec(X, Y)
  {
    static_assert(Dim >= 3);
    Elements_[2] = Z;
  }
  constexpr Vec(T X, T Y, T Z, T W)
      : Vec(X, Y, Z)
  {
    static_assert(Dim >= 4);
    Elements_[3] = W;
  }

  Vec& Normalize()
  {
    auto const length = Length();
    for (i32 i = 0; i < Dim; ++i)
      Elements_[i] /= length;
    return *this;
  }
  Vec Normalized() const
  {
    return Vec(*this).Normalize();
  }
  T Length() const
  {
    if constexpr (std::is_integral_v<T> || std::is_same_v<T, f64>)
      return sqrt(LengthSquared());
    else
      return sqrtf(LengthSquared());
  }
  constexpr T LengthSquared() const
  {
    return *this * *this;
  }
  decltype(auto) Cosine(Vec const& Other) const
  {
    // Due to integer division we might end up with cos(v, u) = 0, while they aren't perpendicular at all
    if constexpr (std::is_integral_v<T>)
      return (f32)Dot(Other) / (Length() * Other.Length());
    else
      return Dot(Other) / (Length() * Other.Length());
  }
  // Faster alternative if you only care about angles of 90 degrees, as it returns values between 0 and 1
  decltype(auto) CosineSquared(Vec const& Other) const
  {
    // Due to integer division we might end up with cos(v, u) = 0, while they aren't perpendicular at all
    if constexpr (std::is_integral_v<T>)
      return (f32)(Dot(Other) * Dot(Other)) / (LengthSquared() * Other.LengthSquared());
    else
      return (Dot(Other) * Dot(Other)) / (LengthSquared() * Other.LengthSquared());
  }
  constexpr T Dot(Vec const& Other) const
  {
    return *this * Other;
  }
  constexpr Vec Cross(Vec const& Other) const
  {
    static_assert(Dim == 2 || Dim == 3);
    if constexpr (Dim == 2)
    {
      /* (v1 v2
       *  w1 w2 )
       */
      return {X() * Other.Y(), Y() * Other.X()};
    }
    else
    {
      /* ( i  j  k
       *  v1 v2 v3
       *  w1 w2 w3 )
       */
      return {(Y() * Other.Z()) - (Z() * Other.Y()), (Z() * Other.X()) - (X() * Other.Z()), (X() * Other.Y()) - (Y() * Other.X())};
    }
  }
  friend constexpr Vec operator+(Vec const& v, Vec const& u)
  {
    Vec cpy;
    for (i32 i = 0; i < Dim; ++i)
      cpy.Elements_[i] = v.Elements_[i] + u.Elements_[i];
    return cpy;
  }
  friend constexpr Vec operator-(Vec const& v, Vec const& u)
  {
    Vec cpy;
    for (i32 i = 0; i < Dim; ++i)
      cpy.Elements_[i] = v.Elements_[i] - u.Elements_[i];
    return cpy;
  }
  friend constexpr T operator*(Vec const& v, Vec const& u)
  {
    T r = 0;
    for (i32 i = 0; i < Dim; ++i)
      r = v.Elements_[i] * u.Elements_[i];
    return r;
  }
  friend constexpr Vec operator*(Vec const& v, T const Scalar)
  {
    Vec cpy;
    for (i32 i = 0; i < Dim; ++i)
      cpy.Elements_[i] = v.Elements_[i] * Scalar;
    return cpy;
  }
  friend constexpr bool operator==(Vec const& v, Vec const& u)
  {
    for (i32 i = 0; i < Dim; ++i)
      if (v.Elements_[i] != u.Elements_[i])
        return false;

    return true;
  }

  constexpr T& X()
  {
    return Elements_[0];
  }
  constexpr T const& X() const
  {
    return Elements_[0];
  }

  constexpr T& Y()
  {
    static_assert(Dim >= 2);
    return Elements_[1];
  }
  constexpr T const& Y() const
  {
    static_assert(Dim >= 2);
    return Elements_[1];
  }

  constexpr T& Z()
  {
    static_assert(Dim >= 3);
    return Elements_[2];
  }
  constexpr T const& Z() const
  {
    static_assert(Dim >= 3);
    return Elements_[2];
  }

  constexpr T& W()
  {
    static_assert(Dim >= 4);
    return Elements_[3];
  }
  constexpr T const& W() const
  {
    static_assert(Dim >= 4);
    return Elements_[3];
  }
};

template <typename T>
using Vec1D = Vec<T, 1>;

template <typename T>
using Vec2D = Vec<T, 2>;

template <typename T>
using Vec3D = Vec<T, 3>;

using Vec1Di = Vec<i32, 1>;

using Vec2Di = Vec<i32, 2>;

using Vec3Di = Vec<i32, 3>;

using Vec1Df = Vec<f32, 1>;

using Vec2Df = Vec<f32, 2>;

using Vec3Df = Vec<f32, 3>;
} // namespace Math
