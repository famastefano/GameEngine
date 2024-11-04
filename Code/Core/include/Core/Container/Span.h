#pragma once

#include <Core/Assert/Assert.h>
#include <Core/Container/Vector.h>
#include <Core/Definitions.h>
#include <iterator>
#include <memory>
#include <type_traits>

namespace Core
{
inline static constexpr i32 DynamicSize = -1;

namespace Private
{
template <i32 Size_>
struct SpanSize
{
  constexpr i32 Size() const
  {
    return Size_;
  }

  constexpr SpanSize(i32 N)
  {
    check(Size_ <= N);
  }
};

template <>
struct SpanSize<DynamicSize>
{
  i32 N_;

  constexpr i32 Size() const
  {
    return N_;
  }

  constexpr SpanSize(i32 N)
      : N_(N)
  {
  }
};
} // namespace Private

template <typename T, i32 Size_ = DynamicSize>
class Span
{
  T* Data_;

  [[no_unique_address]] Private::SpanSize<Size_> SpanSize_;

public:
  constexpr Span() noexcept
      : Data_(nullptr)
      , SpanSize_(0)
  {
    static_assert(Size_ == 0 || Size_ == DynamicSize);
  }

  template <std::contiguous_iterator Iterator>
  explicit(Size_ != DynamicSize) constexpr Span(Iterator start, i32 const count)
      : Data_(std::to_address(start))
      , SpanSize_(count)
  {
    check(Size() == count);
  }

  template <std::contiguous_iterator Iterator>
  explicit(Size_ != DynamicSize) constexpr Span(Iterator start, Iterator end)
      : Data_(std::to_address(start))
      , SpanSize_(i32(end - start))
  {
    check(Size() == (end - start));
  }

  template <typename U>
  explicit constexpr Span(Vector<U> const& vec)
      : Data_(vec.Data())
      , SpanSize_(vec.Size())
  {
  }

  template <typename U>
  explicit constexpr Span(Vector<U>& vec)
      : Data_(vec.Data())
      , SpanSize_(vec.Size())
  {
  }

  explicit(Size_ != DynamicSize) constexpr Span(std::initializer_list<T> il) noexcept
      : Span(il.begin(), i32(il.size()))
  {
  }

  constexpr Span(Span const&) = default;

  constexpr i32 IsEmpty() const
  {
    return Size() == 0;
  }

  constexpr i32 Size() const
  {
    return SpanSize_.Size();
  }

  constexpr i32 AllocSize() const
  {
    return Size() * sizeof(T);
  }

  constexpr T* Data() const
  {
    return Data_;
  }

  constexpr T* begin() const
  {
    return Data_;
  }

  constexpr T* end() const
  {
    return Data_ + SpanSize_.Size();
  }

  template <i32 Count>
  constexpr Span<T, Count> First() const
  {
    if constexpr (Size_ != DynamicSize)
      static_assert(0 <= Count && Count <= Size_);
    else
      check(0 <= Count && Count <= Size());
    return Span<T, Count>(Data_, Count);
  }

  constexpr Span<T, DynamicSize> First(i32 const count) const
  {
    check(0 <= count && count <= Size());
    return Span<T, DynamicSize>(Data_, count);
  }

  template <i32 Count>
  constexpr Span<T, Count> Last() const
  {
    if constexpr (Size_ != DynamicSize)
      static_assert(0 <= Count && Size_ - Count >= 0);
    else
      check(0 <= Count && Size() - Count >= 0);
    return Span<T, Count>(Data_ + Size() - Count, Count);
  }

  constexpr Span<T, DynamicSize> Last(i32 const count) const
  {
    check(0 <= count && Size() - count >= 0);
    return Span<T, DynamicSize>(Data_ + Size() - count, count);
  }

  template <i32 Offset, i32 Count>
  constexpr Span<T, Count> SubSpan() const
  {
    if constexpr (Size_ != DynamicSize)
      static_assert(0 <= Offset + Count && Offset + Count <= Size_);
    else
      check(0 <= Offset + Count && Offset + Count <= Size());
    return Span<T, Count>(Data_ + Offset, Count);
  }

  constexpr Span<T, DynamicSize> SubSpan(i32 const offset, i32 const count) const
  {
    check(0 <= offset + count && offset + count <= Size());
    return Span<T, DynamicSize>(Data_ + offset, count);
  }

  constexpr T& operator[](i32 const offset)
  {
    check(!IsEmpty() && u32(offset) < u32(Size()));
    return Data_[offset];
  }

  constexpr T const& operator[](i32 const offset) const
  {
    check(!IsEmpty() && u32(offset) < u32(Size()));
    return Data_[offset];
  }
};

template <typename T, i32 N>
Span(T (&)[N]) -> Span<T, N>;

template <i32 N>
Span(char (&)[N])->Span<char const, N> const;

template <i32 N>
Span(wchar_t (&)[N])->Span<wchar_t const, N> const;
} // namespace Core
