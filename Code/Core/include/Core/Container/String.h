#pragma once

#include <Core/Algorithm/Algorithm.h>
#include <Core/Allocator/Allocator.h>
#include <Core/Container/Span.h>
#include <Core/Container/Vector.h>
#include <Core/Definitions.h>
#include <cstring>
#include <type_traits>
#include <wchar.h>

namespace Core
{
template <typename CharT>
class StringView : public Span<CharT, DynamicSize>
{
  static_assert(std::same_as<CharT, char> || std::same_as<CharT, wchar_t>, "StringView supports only char and wchar_t.");

public:
  inline static constexpr i32 NotFound = -1;

  using Span<CharT, DynamicSize>::Span;

  constexpr StringView(std::nullptr_t) = delete;

  template <std::enable_if_t<std::same_as<CharT, char>, bool> = true>
  constexpr StringView(CharT const* s)
      : Span(s, strlen(s))
  {
  }

  template <std::enable_if_t<std::same_as<CharT, wchar_t>, bool> = true>
  constexpr StringView(CharT const* s)
      : Span(s, wcslen(s))
  {
  }

  constexpr CharT& Front()
  {
    return operator[](0);
  }

  constexpr CharT const& Front() const
  {
    return operator[](0);
  }

  constexpr CharT& Back()
  {
    constexpr i32 sz = Size();
    return operator[](sz > 0 ? sz - 1 : 0);
  }

  constexpr CharT const& Back() const
  {
    constexpr i32 sz = Size();
    return operator[](sz > 0 ? sz - 1 : 0);
  }

  constexpr StringView RemovePrefix(i32 const length)
  {
    return First(length);
  }

  constexpr StringView RemoveSuffix(i32 const length)
  {
    return First(Size() - length);
  }

  constexpr i32 Copy(CharT* dest, i32 const count, i32 const offset = 0) const
  {
    CharT*    src         = &operator[](offset);
    i32 const maximumCopy = Size() - offset;
    i32 const actualCount = maximumCopy < count ? maximumCopy : count;
    Algorithm::Copy(src, src + actualCount, dest);
    return actualCount;
  }

  constexpr StringView SubStr(i32 const offset, i32 const count) const
  {
    return SubSpan(offset, count);
  }

  constexpr bool StartsWith(StringView const other) const
  {
    if (Size() < other.Size() || other.IsEmpty())
      return false;
    return memcmp(Data(), other.Data(), other.AllocSize()) == 0;
  }

  constexpr bool StartsWith(CharT const c) const
  {
    return !IsEmpty() && Front() == c;
  }

  constexpr bool EndsWith(StringView const other) const
  {
    if (Size() < other.Size() || other.IsEmpty())
      return false;
    return memcmp(Data() + Size() - other.Size(), other.Data(), other.AllocSize()) == 0;
  }

  constexpr bool EndsWith(CharT const c) const
  {
    return !IsEmpty() && Back() == c;
  }

  constexpr bool Contains(CharT const c) const
  {
    return Find(c);
  }

  constexpr bool Contains(StringView const other)
  {
    if (Size() < other.Size() || IsEmpty() || other.IsEmpty())
      return false;

    CharT* p;
    if constexpr (std::same_as<CharT, char>)
    {
      p = memchr(Data(), other[0], Size());
    }
    else
    {
      p = wmemchr(Data(), other[0], Size());
    }

    if (p + other.Size() < Data() + Size())
      return memcmp(p, other.Data(), other.AllocSize()) == 0;
    return false;
  }

  constexpr i32 Find(CharT const c, i32 const offset = 0) const
  {
    if (IsEmpty() || u32(offset) >= u32(Size()))
      return NotFound;

    CharT* p;
    if constexpr (std::same_as<CharT, char>)
    {
      p = memchr(Data() + offset, c, Size());
    }
    else
    {
      p = wmemchr(Data() + offset, c, Size())
    }
    return p ? p - Data() : NotFound;
  }

  constexpr i32 Find(StringView const other, i32 const offset = 0) const
  {
    if (IsEmpty() || u32(offset) >= u32(Size()) || u32(Size()) < other.Size() + u32(offset))
      return NotFound;

    CharT* p;
    if constexpr (std::same_as<CharT, char>)
    {
      p = memchr(Data() + offset, c, Size());
    }
    else
    {
      p = wmemchr(Data() + offset, c, Size())
    }
    if (p && memcmp(p, other.Data(), other.AllocSize()) == 0)
      return p - Data();
    return NotFound;
  }

  constexpr i32 FindFirstOf(CharT const c, i32 const offset = 0) const
  {
    return Find(c, offset);
  }

  constexpr i32 FindFirstOf(StringView const other, i32 const offset = 0) const
  {
    return Find(other, offset);
  }

  constexpr i32 FindLastOf(CharT const c, i32 const offset = 0) const
  {
    if (IsEmpty() || u32(Size()) <= u32(offset))
      return NotFound;

    for (i32 i = Size() - 1; i >= offset; --i)
    {
      if (operator[](i) == c)
        return i;
    }
    return NotFound;
  }

  constexpr i32 FindLastOf(StringView const other, i32 const offset = 0) const
  {
    if (IsEmpty() || other.IsEmpty() || u32(Size()) < u32(other.Size() + offset))
      return NotFound;
       
    for (i32 i = Size() - other.Size() - 1; i >= offset; --i)
    {
      if (operator[](i) == c && memcmp(Data() + i, other.Data(), other.AllocSize()) == 0)
        return i;
    }
    return NotFound;
  }

  constexpr i32 FindFirstNotOf(CharT const c, i32 const offset = 0) const
  {
    return NotFound;
  }

  constexpr i32 FindFirstNotOf(StringView const other, i32 const offset = 0) const
  {
    return NotFound;
  }

  constexpr i32 FindLastNotOf(CharT const c, i32 const offset = 0) const
  {
    return NotFound;
  }

  constexpr i32 FindLastNotOf(StringView const other, i32 const offset = 0) const
  {
    return NotFound;
  }

  constexpr bool operator==(StringView const other) const
  {
    return Size() == other.Size() && memcmp(Data(), other.Data(), AllocSize()) == 0;
  }

  constexpr bool operator!=(StringView const other) const
  {
    return !operator==(other);
  }

  constexpr bool operator<(StringView const other) const
  {
    if (IsEmpty() && other.IsEmpty())
      return false;

    if (IsEmpty())
      return true;

    if (other.IsEmpty())
      return false;

    return memcmp(Data(), other.Data(), AllocSize())
  }
};
} // namespace Core
