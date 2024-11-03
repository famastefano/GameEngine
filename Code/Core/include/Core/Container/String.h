#pragma once

#include <Core/Algorithm/Algorithm.h>
#include <Core/Allocator/Allocator.h>
#include <Core/Container/Span.h>
#include <Core/Container/Vector.h>
#include <Core/Definitions.h>
#include <cstring>
#include <memory>
#include <type_traits>
#include <wchar.h>

namespace Core
{
template <typename CharT>
class StringView : public Span<CharT const, DynamicSize>
{
  static_assert(std::same_as<CharT, char> || std::same_as<CharT, wchar_t>, "StringView supports only char and wchar_t.");

public:
  using Super = Span<CharT const, DynamicSize>;

  inline static constexpr i32 NotFound = -1;

  using Span<CharT const, DynamicSize>::Span;

  template <typename CharU = CharT, std::enable_if_t<std::same_as<CharU, char>, bool> = true>
  constexpr StringView(CharU const* s)
      : Super(s, i32(strlen(s)))
  {
  }

  template <typename CharU = CharT, std::enable_if_t<std::same_as<CharU, wchar_t>, bool> = true>
  constexpr StringView(CharU const* s)
      : Super(s, i32(wcslen(s)))
  {
  }

  constexpr CharT& Front()
  {
    return (*this)[0];
  }

  constexpr CharT const& Front() const
  {
    return (*this)[0];
  }

  constexpr CharT& Back()
  {
    constexpr i32 sz = Super::Size();
    return (*this)[sz > 0 ? sz - 1 : 0];
  }

  constexpr CharT const& Back() const
  {
    constexpr i32 sz = Super::Size();
    return (*this)[sz > 0 ? sz - 1 : 0];
  }

  constexpr StringView RemovePrefix(i32 const length)
  {
    return Super::First(length);
  }

  constexpr StringView RemoveSuffix(i32 const length)
  {
    return Super::First(Super::Size() - length);
  }

  constexpr i32 Copy(CharT* dest, i32 const count, i32 const offset = 0) const
  {
    CharT*    src         = std::addressof((*this)[offset]);
    i32 const maximumCopy = Super::Size() - offset;
    i32 const actualCount = maximumCopy < count ? maximumCopy : count;
    Algorithm::Copy(src, src + actualCount, dest);
    return actualCount;
  }

  constexpr StringView SubStr(i32 const offset, i32 const count) const
  {
    return Super::SubSpan(offset, count);
  }

  constexpr bool StartsWith(StringView const other) const
  {
    if (Super::Size() < other.Size() || other.IsEmpty())
      return false;
    return memcmp(Super::Data(), other.Data(), other.AllocSize()) == 0;
  }

  constexpr bool StartsWith(CharT const c) const
  {
    return !Super::IsEmpty() && Super::Front() == c;
  }

  constexpr bool EndsWith(StringView const other) const
  {
    if (Super::Size() < other.Size() || other.IsEmpty())
      return false;
    return memcmp(Super::Data() + Super::Size() - other.Size(), other.Data(), other.AllocSize()) == 0;
  }

  constexpr bool EndsWith(CharT const c) const
  {
    return !Super::IsEmpty() && Super::Back() == c;
  }

  constexpr bool Contains(CharT const c) const
  {
    return Super::Find(c);
  }

  constexpr bool Contains(StringView const other)
  {
    if (Super::Size() < other.Size() || Super::IsEmpty() || other.IsEmpty())
      return false;

    CharT* p;
    if constexpr (std::same_as<CharT, char>)
    {
      p = memchr(Super::Data(), other[0], Super::Size());
    }
    else
    {
      p = wmemchr(Super::Data(), other[0], Super::Size());
    }

    if (p + other.Size() < Super::Data() + Super::Size())
      return memcmp(p, other.Data(), other.AllocSize()) == 0;
    return false;
  }

  constexpr i32 Find(CharT const c, i32 const offset = 0) const
  {
    if (Super::IsEmpty() || u32(offset) >= u32(Super::Size()))
      return NotFound;

    CharT* p;
    if constexpr (std::same_as<CharT, char>)
    {
      p = memchr(Super::Data() + offset, c, Super::Size());
    }
    else
    {
      p = wmemchr(Super::Data() + offset, c, Super::Size());
    }
    return p ? p - Super::Data() : NotFound;
  }

  constexpr i32 Find(StringView const other, i32 const offset = 0) const
  {
    if (Super::IsEmpty() || u32(offset) >= u32(Super::Size()) || u32(Super::Size()) < other.Size() + u32(offset))
      return NotFound;

    CharT* p;
    if constexpr (std::same_as<CharT, char>)
    {
      p = memchr(Super::Data() + offset, other[0], Super::Size());
    }
    else
    {
      p = wmemchr(Super::Data() + offset, other[0], Super::Size());
    }
    if (p && memcmp(p, other.Data(), other.AllocSize()) == 0)
      return p - Super::Data();
    return NotFound;
  }

  constexpr i32 FindFirstOf(CharT const c, i32 const offset = 0) const
  {
    return Super::Find(c, offset);
  }

  constexpr i32 FindFirstOf(StringView const other, i32 const offset = 0) const
  {
    return Super::Find(other, offset);
  }

  constexpr i32 FindLastOf(CharT const c, i32 const offset = 0) const
  {
    if (Super::IsEmpty() || u32(Super::Size()) <= u32(offset))
      return NotFound;

    for (i32 i = Super::Size() - 1; i >= offset; --i)
    {
      if (operator[](i) == c)
        return i;
    }
    return NotFound;
  }

  constexpr i32 FindLastOf(StringView const other, i32 const offset = 0) const
  {
    if (Super::IsEmpty() || other.IsEmpty() || u32(Super::Size()) < u32(other.Size() + offset))
      return NotFound;

    for (i32 i = Super::Size() - other.Size() - 1; i >= offset; --i)
    {
      if ((*this)[i] == other[i] && memcmp(Super::Data() + i, other.Data(), other.AllocSize()) == 0)
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
    return Super::Size() == other.Size() && memcmp(Super::Data(), other.Data(), Super::AllocSize()) == 0;
  }

  constexpr bool operator!=(StringView const other) const
  {
    return !operator==(other);
  }

  constexpr bool operator<(StringView const other) const
  {
    if (Super::IsEmpty() && other.IsEmpty())
      return false;

    if (Super::IsEmpty())
      return true;

    if (other.IsEmpty())
      return false;

    return memcmp(Super::Data(), other.Data(), Super::AllocSize());
  }
};
} // namespace Core
