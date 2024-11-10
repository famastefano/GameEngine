#pragma once

#include <Core/Container/Vector.h>
#include <Core/Definitions.h>
#include <algorithm>

namespace Core
{
template <typename T>
concept Sortable = requires(T a, T b) {
  a < b ? 0 : 1;
};

template <typename Key, typename Value>
struct KeyValue
{
  Key&   Key_;
  Value& Value_;
};

template <typename Key, typename Value>
class FlatMapIterator
{
private:
  Key*   Key_;
  Value* Value_;

public:
  using KV = KeyValue<Key, Value>;

  constexpr FlatMapIterator()
      : FlatMapIterator(nullptr, nullptr)
  {
  }

  constexpr FlatMapIterator(Key* key, Value* value)
      : Key_(key)
      , Value_(value)
  {
  }

  constexpr FlatMapIterator& operator++()
  {
    ++Key_;
    ++Value_;
    return *this;
  }

  constexpr FlatMapIterator operator++(i32) const
  {
    FlatMapIterator cpy(*this);
    ++Key_;
    ++Value_;
    return cpy;
  }

  constexpr FlatMapIterator& operator--()
  {
    --Key_;
    --Value_;
    return *this;
  }

  constexpr FlatMapIterator operator--(i32) const
  {
    FlatMapIterator cpy(*this);
    --Key_;
    --Value_;
    return cpy;
  }

  constexpr FlatMapIterator& operator+=(i32 const n)
  {
    Key_   += n;
    Value_ += n;
    return *this;
  }

  constexpr FlatMapIterator& operator-=(i32 const n)
  {
    Key_   -= n;
    Value_ -= n;
    return *this;
  }

  constexpr KV operator[](i32 const n) const
  {
    return KV{Key_[n], Value_[n]};
  }

  constexpr KV operator*() const
  {
    return (*this)[0];
  }

  constexpr KV operator->() const
  {
    return *(*this);
  }

  constexpr friend FlatMapIterator operator+(FlatMapIterator it, i32 const n)
  {
    return (it += n);
  }

  constexpr friend FlatMapIterator operator+(i32 const n, FlatMapIterator it)
  {
    return (it += n);
  }

  constexpr FlatMapIterator operator-(i32 const n)
  {
    return (FlatMapIterator(*this) -= n);
  }

  constexpr bool operator<(FlatMapIterator other) const
  {
    return Key_ < other.Key_;
  }
  constexpr bool operator>(FlatMapIterator other) const
  {
    return Key_ > other.Key_;
  }
  constexpr bool operator==(FlatMapIterator other) const
  {
    return Key_ == other.Key_;
  }
  constexpr bool operator!=(FlatMapIterator other) const
  {
    return Key_ != other.Key_;
  }
};

template <Sortable Key, typename Value>
class FlatMap
{
  Vector<Key>   Keys_;
  Vector<Value> Values_;

public:
  constexpr FlatMap(IAllocator* allocator = globalAllocator)
      : Keys_(allocator)
      , Values_(allocator)
  {
  }

  FlatMapIterator<Key const, Value> begin()
  {
    return {Keys_.begin(), Values_.begin()};
  }
  FlatMapIterator<Key const, Value const> begin() const
  {
    return {Keys_.begin(), Values_.begin()};
  }

  FlatMapIterator<Key const, Value> end()
  {
    return {Keys_.end(), Values_.end()};
  }
  FlatMapIterator<Key const, Value const> end() const
  {
    return {Keys_.end(), Values_.end()};
  }

  constexpr Vector<Key> const& Keys() const
  {
    return Keys_;
  }

  constexpr Vector<Value>& Values()
  {
    return Values_;
  }

  constexpr Vector<Value> const& Values() const
  {
    return Values_;
  }

  template <typename U = Key>
  constexpr Value& operator[](U const& u)
  {
    auto* found = Find(u);
    check(found);
    return *found;
  }

  template <typename U = Key>
  constexpr Value const& operator[](U const& u) const
  {
    auto* found = Find(u);
    check(found);
    return *found;
  }

  template <typename U = Key>
  constexpr bool Contains(U const& u) const
  {
    return Find(u);
  }

  template <typename U = Key>
  constexpr Value const* Find(U const& u) const
  {
    auto begin = Keys_.begin();
    auto end   = Keys_.end();
    auto it    = std::lower_bound(begin, end, u);
    if (it != end && *it == u)
    {
      Value const* v = Values_.Data() + (it - begin);
      return v;
    }
    return nullptr;
  }

  template <typename U = Key>
  constexpr Value* Find(U const& u)
  {
    FlatMap const& selfConst = *this;
    return const_cast<Value*>(selfConst.Find(u));
  }

  template <typename U = Key, typename... Args>
  constexpr bool TryEmplace(U&& key, Args&&... args)
  {
    auto begin = Keys_.begin();
    auto end   = Keys_.end();
    auto it    = std::lower_bound(begin, end, key);
    if (it != end && *it == key)
      return false;

    i32 const pos = i32(it - begin);
    Keys_.Emplace(pos, std::forward<U>(key));
    Values_.Emplace(pos, std::forward<Args>(args)...);
    return true;
  }

  template <typename U = Key>
  constexpr bool TryRemove(U&& key)
  {
    auto begin = Keys_.begin();
    auto end   = Keys_.end();
    auto it    = std::lower_bound(begin, end, key);
    if (it != end || *it != key)
      return false;

    i32 const pos = i32(it - begin);
    Keys_.Erase(pos);
    Values_.Erase(pos);
    return true;
  }
};
} // namespace Core
