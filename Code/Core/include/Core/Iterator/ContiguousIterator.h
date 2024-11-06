#pragma once

#include <Core/Definitions.h>

namespace Core
{
template <typename T>
class ReverseContiguousIterator
{
  T* Ptr_;

public:
  constexpr ReverseContiguousIterator()
      : Ptr_(nullptr)
  {
  }

  constexpr ReverseContiguousIterator(T* p)
      : Ptr_(p)
  {
  }

  constexpr operator T*() const
  {
    return Ptr_;
  }

  constexpr ReverseContiguousIterator& operator++()
  {
    --Ptr_;
    return *this;
  }

  constexpr ReverseContiguousIterator operator++(i32) const
  {
    return Ptr_--;
  }

  constexpr ReverseContiguousIterator& operator--()
  {
    ++Ptr_;
    return *this;
  }

  constexpr ReverseContiguousIterator operator--(i32) const
  {
    return Ptr_++;
  }

  constexpr ReverseContiguousIterator& operator+=(i32 const n)
  {
    Ptr_ -= n;
    return *this;
  }

  constexpr ReverseContiguousIterator& operator-=(i32 const n)
  {
    Ptr_ += n;
    return *this;
  }

  constexpr T& operator[](i32 const n) const
  {
    return Ptr_[-n];
  }

  constexpr T& operator*() const
  {
    return *Ptr_;
  }

  constexpr T& operator->() const
  {
    return *Ptr_;
  }

  constexpr friend ReverseContiguousIterator operator+(ReverseContiguousIterator it, i32 const n)
  {
    return (it += n);
  }

  constexpr friend ReverseContiguousIterator operator+(i32 const n, ReverseContiguousIterator it)
  {
    return (it += n);
  }

  constexpr ReverseContiguousIterator operator-(i32 const n)
  {
    return (ReverseContiguousIterator(Ptr_) += n);
  }

  constexpr bool operator<=>(ReverseContiguousIterator other) const
  {
    return Ptr_ <=> other.Ptr_;
  }
};
} // namespace Core
