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

  constexpr CharT const& Front() const
  {
    return (*this)[0];
  }

  constexpr CharT const& Back() const
  {
    i32 const sz = Super::Size();
    return (*this)[sz > 0 ? sz - 1 : 0];
  }

  constexpr StringView RemovePrefix(i32 const length)
  {
    auto const span = Super::Last(Super::Size() - length);
    return StringView(span.Data(), span.Size());
  }

  constexpr StringView RemoveSuffix(i32 const length)
  {
    auto const span = Super::First(Super::Size() - length);
    return StringView(span.Data(), span.Size());
  }

  constexpr i32 Copy(CharT* dest, i32 const count, i32 const offset = 0) const
  {
    check(dest && count >= 0 && offset >= 0);
    CharT const* src         = Super::Data() + offset;
    i32 const    maximumCopy = Super::Size() - offset;
    i32 const    actualCount = maximumCopy < count ? maximumCopy : count;
    check(src + actualCount <= Super::Data() + Super::Size());
    Algorithm::Copy(src, src + actualCount, dest);
    return actualCount;
  }

  constexpr StringView SubStr(i32 const offset, i32 const count) const
  {
    auto const span = Super::SubSpan(offset, count);
    return StringView(span.Data(), span.Size());
  }

  constexpr bool StartsWith(StringView const other) const
  {
    if (Super::Size() < other.Size() || Super::IsEmpty() || other.IsEmpty())
      return false;
    return memcmp(Super::Data(), other.Data(), (u64)other.AllocSize()) == 0;
  }

  constexpr bool StartsWith(CharT const c) const
  {
    return !Super::IsEmpty() && Super::Front() == c;
  }

  constexpr bool EndsWith(StringView const other) const
  {
    if (Super::Size() < other.Size() || Super::IsEmpty() || other.IsEmpty())
      return false;
    return memcmp(Super::Data() + Super::Size() - other.Size(), other.Data(), (u64)other.AllocSize()) == 0;
  }

  constexpr bool EndsWith(CharT const c) const
  {
    return !Super::IsEmpty() && Super::Back() == c;
  }

  constexpr bool Contains(CharT const c) const
  {
    return Find(c) != NotFound;
  }

  constexpr bool Contains(StringView const other)
  {
    return Find(other) != NotFound;
  }

  constexpr i32 Find(CharT const c, i32 const offset = 0) const
  {
    if (Super::IsEmpty() || u32(offset) >= u32(Super::Size()))
      return NotFound;

    CharT const* p;
    if constexpr (std::same_as<CharT, char>)
    {
      p = (CharT const*)memchr(Super::Data() + offset, c, (u64)Super::Size());
    }
    else
    {
      p = wmemchr(Super::Data() + offset, c, (u64)Super::Size());
    }
    return p ? i32(p - Super::Data()) : NotFound;
  }

  constexpr i32 Find(StringView const other, i32 const offset = 0) const
  {
    if (Super::IsEmpty() || other.IsEmpty() || u32(offset) >= u32(Super::Size()) || u32(Super::Size()) < other.Size() + u32(offset))
      return NotFound;

    i32 pos = 0;
    while (true)
    {
      i32 const foundPos = Find(other[0], pos);
      if (NotFound == foundPos)
        return NotFound;

      if (foundPos + other.Size() > Super::Size())
        return NotFound;

      CharT const* p = Super::Data() + foundPos;
      if (memcmp(p, other.Data(), (u64)other.AllocSize()) == 0)
        return foundPos;

      pos = foundPos + 1;
    }
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
      if (Super::operator[](i) == c)
        return i;
    }
    return NotFound;
  }

  constexpr i32 FindLastOf(StringView const other, i32 const offset = 0) const
  {
    if (Super::IsEmpty() || other.IsEmpty() || u32(Super::Size()) < u32(other.Size() + offset))
      return NotFound;

    for (i32 i = Super::Size() - other.Size(); i >= offset; --i)
    {
      if ((*this)[i] == other[0] && memcmp(Super::Data() + i, other.Data(), (u64)other.AllocSize()) == 0)
        return i;
    }
    return NotFound;
  }

  constexpr bool operator==(StringView const other) const
  {
    return Super::Size() == other.Size() && memcmp(Super::Data(), other.Data(), (u64)Super::AllocSize()) == 0;
  }

  constexpr bool operator!=(StringView const other) const
  {
    return !operator==(other);
  }

  constexpr bool operator<(StringView const other) const
  {
    if (Super::IsEmpty() && !other.IsEmpty())
      return true;

    if (!Super::IsEmpty() && other.IsEmpty())
      return false;

    return memcmp(Super::Data(), other.Data(), Super::AllocSize()) == -1;
  }
};

template <typename CharT>
class String
{
  inline static constexpr CharT Terminator = CharT('\0');

  Vector<CharT> Mem_;

public:
  constexpr String(IAllocator* allocator = globalAllocator)
      : Mem_(allocator)
  {
  }

  constexpr String(i32 const count, char const c, IAllocator* allocator = globalAllocator)
      : String(allocator)
  {
    Assign(c, count);
  }

  constexpr String(StringView<CharT> view, IAllocator* allocator = globalAllocator)
      : String(allocator)
  {
    Assign(view);
  }

  constexpr String(String const& other)
      : Mem_(other.Mem_)
  {
  }

  constexpr String(String&& other)
      : Mem_(std::move(other.Mem_))
  {
  }

  constexpr String& operator=(String const& other)
  {
    Mem_ = other.Mem_;
    return *this;
  }

  constexpr String& operator=(String&& other)
  {
    Mem_ = std::move(other.Mem_);
    return *this;
  }

  constexpr IAllocator* Allocator() const
  {
    return Mem_.Allocator();
  }

  constexpr bool IsEmpty() const
  {
    return Mem_.IsEmpty();
  }

  constexpr i32 Size() const
  {
    return Mem_.IsEmpty() ? 0 : Mem_.Size() - 1; // we exclude the terminator
  }

  constexpr i32 Capacity() const
  {
    return Mem_.Capacity();
  }

  constexpr i32 AllocSize() const
  {
    return Mem_.AllocSize();
  }

  constexpr CharT* Data()
  {
    return Mem_.Data();
  }
  constexpr CharT const* Data() const
  {
    return Mem_.Data();
  }

  constexpr CharT* begin()
  {
    return Mem_.begin();
  }
  constexpr CharT const* begin() const
  {
    return Mem_.begin();
  }

  constexpr CharT* end()
  {
    return Mem_.end();
  }
  constexpr CharT const* end() const
  {
    return Mem_.end();
  }

  constexpr CharT& Front()
  {
    return Mem_.Front();
  }
  constexpr CharT const& Front() const
  {
    return Mem_.Front();
  }

  constexpr CharT& Back()
  {
    return Mem_.Back();
  }
  constexpr CharT const& Back() const
  {
    return Mem_.Back();
  }

  constexpr CharT& operator[](i32 const pos)
  {
    return Mem_[pos];
  }
  constexpr CharT const& operator[](i32 const pos) const
  {
    return Mem_[pos];
  }

  constexpr operator StringView<CharT>() const&
  {
    return StringView<CharT>(Data(), Size());
  }

  constexpr StringView<CharT> AsView() const
  {
    return StringView<CharT>(Data(), Size());
  }

  constexpr void Assign(StringView<CharT> view)
  {
    bool const needsTerminator = !view.IsEmpty() && view.Back() != Terminator;
    Mem_.Reserve(view.Size() + (needsTerminator ? 1 : 0));
    Mem_.Assign(view.begin(), view.end());
    if (needsTerminator)
      Mem_.EmplaceBackUnsafe(Terminator);
  }

  constexpr void Assign(CharT const c, i32 const count)
  {
    bool const needsTerminator = c != Terminator;
    Mem_.Reserve(count + (needsTerminator ? 1 : 0));
    Mem_.Assign(count, c);
    if (needsTerminator)
      Mem_.EmplaceBackUnsafe(Terminator);
  }

  constexpr void Resize(i32 const newSize, CharT const c = Terminator)
  {
    Mem_.Resize(newSize, c);
  }

  constexpr void Reserve(i32 const newCapacity)
  {
    Mem_.Reserve(newCapacity);
  }

  constexpr void Clear()
  {
    Mem_.Clear();
  }

  constexpr CharT* Insert(CharT const* pos, StringView<CharT> string)
  {
    return Mem_.Insert(pos == end() ? pos - 1 : pos, string.begin(), string.end());
  }

  constexpr CharT* Insert(i32 const pos, StringView<CharT> string)
  {
    return Insert(Data() + pos, string);
  }

  constexpr CharT* Erase(CharT* position)
  {
    if (IsEmpty() || position == end())
      return end();

    return Erase(position, position + 1);
  }

  constexpr CharT* Erase(CharT* begin, CharT* end)
  {
    check(Data() <= begin && end <= Data() + Size());
    if (begin == end() || begin && *begin == Terminator)
      return end();

    return Mem_.Erase(begin, end);
  }

  constexpr CharT* Erase(i32 const position)
  {
    return Erase(begin() + position);
  }

  constexpr CharT* Erase(i32 const position, i32 const count)
  {
    return Erase(begin() + position, begin() + position + count);
  }

  constexpr void PushBack(StringView<CharT> string)
  {
    Insert(end(), string);
  }

  constexpr void PopBack()
  {
    Mem_.PopBack();
  }

  constexpr String operator+(StringView<CharT> other) const
  {
    String cpy(*this);
    cpy += other;
    return cpy;
  }

  constexpr String& operator+=(StringView<CharT> other)
  {
    PushBack(other);
    return *this;
  }

  constexpr bool operator==(StringView<CharT> other) const
  {
    return AsView() == other;
  }

  constexpr bool operator!=(StringView<CharT> other) const
  {
    return AsView() != other;
  }

  constexpr bool operator<(StringView<CharT> other) const
  {
    return AsView() < other;
  }

  u64 CalculateHash() const
  {
    return AsView().CalculateHash();
  }
};
} // namespace Core
