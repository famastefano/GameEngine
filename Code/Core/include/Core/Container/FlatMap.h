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

namespace Private
{
template <typename Key, typename Value>
struct KeyValueRef
{
  Key&   Key_;
  Value& Value_;
};

constexpr int KeyValueIterator = 0;
constexpr int KeyIterator      = 1;
constexpr int ValueIterator    = 2;
} // namespace Private

template <typename Key, typename Value>
class FlatMapIterator
{
private:
  Key*   Key_;
  Value* Value_;

public:
  using KV = Private::KeyValueRef<Key, Value>;

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

  constexpr void Reserve(i32 Capacity)
  {
    Keys_.Reserve(Capacity);
    Values_.Reserve(Capacity);
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

template <typename KV, typename Key, typename Value, int Kind>
class CompactFlatMapIterator;

template <typename KV, typename Key, typename Value, int Kind>
class CompactFlatMapFakeContainer;

template <Sortable Key, typename Value>
class CompactFlatMap
{
public:
  struct KeyValue
  {
    Key   Key_;
    Value Value_;

    KeyValue() = default;

    template <typename UKey = Key, typename... UArgs>
    KeyValue(UKey&& K, UArgs&&... Vs)
        : Key_(std::forward<UKey>(K))
        , Value_(std::forward<UArgs>(Vs)...)
    {
    }

    template <typename T>
    bool operator<(T const& TKey) const
    {
      return Key_ < TKey;
    }

    template <typename T>
    bool operator==(T const& TKey) const
    {
      return Key_ == TKey;
    }
  };

private:
  Vector<KeyValue> Items_;

public:
  constexpr CompactFlatMap(IAllocator* allocator = globalAllocator)
      : Items_(allocator)
  {
  }

  CompactFlatMapIterator<KeyValue, Key const, Value, Private::KeyValueIterator> begin()
  {
    return {Items_.begin()};
  }

  CompactFlatMapIterator<KeyValue, Key const, Value const, Private::KeyValueIterator> begin() const
  {
    return Items_.begin();
  }

  CompactFlatMapIterator<KeyValue, Key const, Value, Private::KeyValueIterator> end()
  {
    return {Items_.end()};
  }

  CompactFlatMapIterator<KeyValue, Key const, Value const, Private::KeyValueIterator> end() const
  {
    return {Items_.end()};
  }

  constexpr CompactFlatMapFakeContainer<KeyValue, Key const, Value, Private::KeyIterator> Keys() const
  {
    return {begin(), end()};
  }

  constexpr CompactFlatMapFakeContainer<KeyValue, Key const, Value, Private::ValueIterator> Values()
  {
    return {begin(), end()};
  }

  constexpr CompactFlatMapFakeContainer<KeyValue, Key const, Value const, Private::ValueIterator> Values() const
  {
    return {begin(), end()};
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
    auto begin = Items_.begin();
    auto end   = Items_.end();
    auto it    = std::lower_bound(begin, end, u);
    if (it != end && *it == u)
      return &it->Value_;
    return nullptr;
  }

  template <typename U = Key>
  constexpr Value* Find(U const& u)
  {
    CompactFlatMap const& selfConst = *this;
    return const_cast<Value*>(selfConst.Find(u));
  }

  constexpr void Reserve(i32 Capacity)
  {
    Items_.Reserve(Capacity);
  }

  template <typename U = Key, typename... Args>
  constexpr bool TryEmplace(U&& key, Args&&... args)
  {
    auto begin = Items_.begin();
    auto end   = Items_.end();
    auto it    = std::lower_bound(begin, end, key);
    if (it != end && *it == key)
      return false;

    Items_.Emplace(it, std::forward<U>(key), std::forward<Args>(args)...);
    return true;
  }

  template <typename U = Key>
  constexpr bool TryRemove(U&& key)
  {
    auto begin = Items_.begin();
    auto end   = Items_.end();
    auto it    = std::lower_bound(begin, end, key);
    if (it != end || *it != key)
      return false;

    Items_.Erase(it);
    return true;
  }
};

template <typename KV, typename Key, typename Value, int Kind>
class CompactFlatMapIterator
{
  KV* Ptr_;

public:
  constexpr CompactFlatMapIterator()
      : CompactFlatMapIterator(nullptr)
  {
  }

  constexpr CompactFlatMapIterator(KV* Ptr)
      : Ptr_(Ptr)
  {
  }

  constexpr CompactFlatMapIterator& operator++()
  {
    ++Ptr_;
    return *this;
  }

  constexpr CompactFlatMapIterator operator++(i32) const
  {
    CompactFlatMapIterator cpy(*this);
    ++cpy;
    return cpy;
  }

  constexpr CompactFlatMapIterator& operator--()
  {
    --Ptr_;
    return *this;
  }

  constexpr CompactFlatMapIterator operator--(i32) const
  {
    CompactFlatMapIterator cpy(*this);
    --cpy;
    return cpy;
  }

  constexpr CompactFlatMapIterator& operator+=(i32 const n)
  {
    Ptr_ += n;
    return *this;
  }

  constexpr CompactFlatMapIterator& operator-=(i32 const n)
  {
    Ptr_ -= n;
    return *this;
  }

  constexpr decltype(auto) operator[](i32 const n) const
  {
    if constexpr (Kind == Private::KeyValueIterator)
      return Ptr_[n];
    else if constexpr (Kind == Private::KeyIterator)
      return static_cast<Key&>(Ptr_[n].Key_);
    else
      return static_cast<Value&>(Ptr_[n].Value_);
  }

  constexpr decltype(auto) operator*() const
  {
    return (*this)[0];
  }

  constexpr decltype(auto) operator->() const
  {
    return *(*this);
  }

  constexpr friend CompactFlatMapIterator operator+(CompactFlatMapIterator it, i32 const n)
  {
    return (it += n);
  }

  constexpr CompactFlatMapIterator operator-(i32 const n)
  {
    return (CompactFlatMapIterator(*this) -= n);
  }

  constexpr bool operator<(CompactFlatMapIterator other) const
  {
    return Ptr_ < other.Ptr_;
  }

  constexpr bool operator>(CompactFlatMapIterator other) const
  {
    return Ptr_ > other.Ptr_;
  }

  constexpr bool operator==(CompactFlatMapIterator other) const
  {
    return Ptr_ == other.Ptr_;
  }
};

template <typename KV, typename Key, typename Value, int Kind>
class CompactFlatMapFakeContainer
{
  KV* Curr_;
  KV* End_;

public:
  CompactFlatMapFakeContainer(KV* Curr, KV* End)
      : Curr_(Curr)
      , End_(End)
  {
  }

  constexpr decltype(auto) begin() const
  {
    return CompactFlatMapIterator<KV, Key, Value, Kind>{Curr_};
  }

  constexpr decltype(auto) end() const
  {
    return CompactFlatMapIterator<KV, Key, Value, Kind>{Curr_};
  }
};
} // namespace Core
