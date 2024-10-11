#pragma once

#include <concepts>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <Core/Allocator/Allocator.h>
#include <Core/Assert/Assert.h>
#include <Core/Definitions.h>
#include <utility>

namespace Core
{
// Dynamic array
template <typename T>
class Vector
{
private:
  IAllocator* Allocator_;
  T*          Mem_;
  T*          Size_;
  T*          Capacity_;

public:
  Vector(IAllocator* allocator = globalAllocator);
  Vector(i32 const initialSize, IAllocator* allocator = globalAllocator);

  template <typename U = T>
  Vector(std::initializer_list<U> init, IAllocator* allocator = globalAllocator);

  template <typename U = T>
  Vector(i32 const initialSize, U const& initialValue, IAllocator* allocator = globalAllocator);

  template <std::input_iterator Iterator>
  Vector(Iterator begin, Iterator end, IAllocator* allocator = globalAllocator);

  Vector(Vector const& other);
  Vector(Vector const& other, IAllocator* allocator);

  Vector(Vector&& other);

  ~Vector();

  Vector& operator=(Vector const& other);
  Vector& operator=(Vector&& other);

  IAllocator* Allocator() const;
  bool        TrySwapAllocator(IAllocator* other);

  template <typename U = T>
  void Assign(i32 const newSize, U const& newValue);

  template <std::input_iterator Iterator>
  void Assign(Iterator begin, Iterator end);

  T&       operator[](i32 const pos);
  T const& operator[](i32 const pos) const;

  T&       Front();
  T const& Front() const;

  T&       Back();
  T const& Back() const;

  T*       Data();
  T const* Data() const;

  T*       begin();
  T const* begin() const;

  T*       end();
  T const* end() const;

  bool IsEmpty() const;

  i32 Size() const;

  i32 Capacity() const;

  void Reserve(i32 const capacity);

  void Resize(i32 const newSize);

  template <typename U = T>
  void Resize(i32 const newSize, U const& value);

  void Swap(Vector& other);

  void Clear();

  template <typename U = T>
  T* Insert(T const* position, U&& value);

  template <typename U = T>
  T* Insert(T const* position, i32 const count, U const& value);

  template <std::input_iterator Iterator>
  T* Insert(T const* position, Iterator begin, Iterator end);

  template <typename... Args>
  T* Emplace(T const* position, Args&&... args);
  template <typename... Args>
  T* EmplaceBack(Args&&... args)
  {
    return Emplace(end(), std::forward<Args>(args)...);
  }

  template <typename... Args>
  T* EmplaceBackUnsafe(Args&&... args);

  T* Erase(T const* position);
  T* Erase(T* begin, T* end);
  T* Erase(i32 const position);
  T* Erase(i32 const position, i32 const count);

  void PopBack();

  template <typename U = T>
  bool Contains(U&& value);

  template <typename U = T>
  T* Find(U&& value);

  template <typename U = T>
  bool operator==(Vector<U> const& other);

  template <typename U = T>
  bool operator!=(Vector<U> const& other);

  template <typename U = T>
  bool operator<(Vector<U> const& other);

  template <typename U = T>
  bool operator>(Vector<U> const& other);

  template <typename U = T>
  bool operator<=(Vector<U> const& other);

  template <typename U = T>
  bool operator>=(Vector<U> const& other);
};

template <typename T>
inline Vector<T>::Vector(IAllocator* allocator)
    : Allocator_(allocator)
    , Mem_(nullptr)
    , Size_(nullptr)
    , Capacity_(nullptr)
{
  check(allocator, "Invalid allocator!");
}

template <typename T>
inline Vector<T>::Vector(i32 const initialSize, IAllocator* allocator)
    : Vector(allocator)
{
  static_assert(std::default_initializable<T>, "T isn't default constructible.");
  if (initialSize)
  {
    Mem_  = allocator->Alloc(initialSize * sizeof(T), alignof(T));
    Size_ = Capacity_ = Mem_ + initialSize;
    check(Mem_, "Failed to allocate enough memory to construct the Vector");
    if constexpr (std::is_trivially_default_constructible_v<T>)
    {
      std::memset(Mem_, 0, Size_ * sizeof(T));
    }
    else
    {
      for (T* item = Mem_; item < Size_; ++item)
        new (item) T;
    }
  }
}

template <typename T>
template <typename U>
inline Vector<T>::Vector(std::initializer_list<U> init, IAllocator* allocator)
    : Vector(allocator)
{
  static_assert(std::constructible_from<T, U>, "Cannot construct T from U.");
  u64 const sz = init.size();
  if (sz)
  {
    Mem_  = allocator->Alloc(sz * sizeof(T), alignof(T));
    Size_ = Capacity_ = Mem_ + sz;
    check(Mem_, "Failed to allocate enough memory to construct the Vector");
  }

  T* item = Mem_;
  for (auto&& value : init)
    new (item++) T(std::forward<U>(value));
}

template <typename T>
template <typename U>
inline Vector<T>::Vector(i32 const initialSize, U const& initialValue, IAllocator* allocator)
    : Vector(initialValue, allocator)
{
  static_assert(std::constructible_from<T, U>, "Cannot construct T from U.");
  if (initialSize)
  {
    // TODO: Refactor with Realloc(size)
    Mem_  = allocator->Alloc(initialSize * sizeof(T), alignof(T));
    Size_ = Capacity_ = Mem_ + initialSize;
    check(Mem_, "Failed to allocate enough memory to construct the Vector");
  }

  for (T* item = Mem_; item < Size_; ++item)
    new (item) T(initialValue);
}

template <typename T>
template <std::input_iterator Iterator>
inline Vector<T>::Vector(Iterator begin, Iterator end, IAllocator* allocator)
    : Vector(allocator)
{
  if constexpr (std::is_trivially_copyable_v<T> && std::is_same_v<std::remove_cvref_t<Iterator>, T*>)
  {
    auto const initialSize = end - begin;
    if (initialSize)
    {
      Mem_  = allocator->Alloc(initialSize * sizeof(T), alignof(T));
      Size_ = Capacity_ = Mem_ + initialSize;
      check(Mem_, "Failed to allocate enough memory to construct the Vector");
      std::memcpy(Mem_, begin, Size_ * sizeof(T));
    }
  }
  else
  {
    auto const initialSize = std::distance(begin, end);
    if (initialSize > 0)
    {
      Mem_  = allocator->Alloc(initialSize * sizeof(T), alignof(T));
      Size_ = Capacity_ = Mem_ + initialSize;
      check(Mem_, "Failed to allocate enough memory to construct the Vector");
      for (T* item = Mem_; item < Size_; ++item)
        new (item) T(*begin++);
    }
  }
}

template <typename T>
inline Vector<T>::Vector(Vector const& other)
    : Vector(other, other.Allocator())
{
}

template <typename T>
inline Vector<T>::Vector(Vector const& other, IAllocator* allocator)
    : Vector(other.begin(), other.end(), allocator)
{
}

template <typename T>
inline Vector<T>::Vector(Vector&& other)
{
  if (other.Allocator()->FollowsContainerDuringMove())
  {
    Mem_       = std::exchange(other.Mem_, nullptr);
    Capacity_  = std::exchange(other.Capacity_, nullptr);
    Size_      = std::exchange(other.Size_, nullptr);
    Allocator_ = std::exchange(other.Allocator_, globalAllocator);
  }
  else
  {
    // TODO: Add logging - Warn about a copy being done instead of a move
    Allocator_ = globalAllocator;

    // We still allocate the entire capacity to keep the behavior as similar as possible
    Mem_       = Allocator_->Alloc(other.Capacity_ * sizeof(T), alignof(T));
    Capacity_  = Mem_ + other.Capacity_;
    Size_      = Mem_ + other.Size_;
    check(Mem_, "Failed to allocate enough memory to construct the Vector");
    Assign(other.begin(), other.end());
    // TODO: destroy items of other vector
  }
}

template <typename T>
inline Vector<T>::~Vector()
{
  if constexpr(!std::is_trivially_destructible_v<T>)
  {
    for(auto& item : *this)
      item->~T();
  }
  Allocator_->Free(Mem_);
  if(Allocator_->OwnedByContainer())
    delete Allocator_;
}
template <typename T>
inline Vector<T>& Vector<T>::operator=(Vector const& other)
{
  // TODO: Refactor with Destroy(from, to)
  if constexpr(!std::is_trivially_destructible_v<T>)
  {
    for(auto& item : *this)
      item->~T();
  }
  Allocator_->Free(Mem_);
  if(!other.IsEmpty())
  {
  
  }
  return *this;
}
} // namespace Core
