#pragma once

#include <Core/Algorithm/Algorithm.h>
#include <Core/Allocator/Allocator.h>
#include <Core/Assert/Assert.h>
#include <Core/Definitions.h>
#include <concepts>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <utility>

namespace Core
{
// Dynamic array
template <typename T>
class Vector
{
private:
  inline static constexpr float ReallocRatio = 1.5f;

  IAllocator* Allocator_;
  T*          Mem_;
  T*          Size_;
  T*          Capacity_;

  void        Reset();
  void        Realloc(i32 const newCapacity);
  static void Destroy(T* from, T* to);

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
inline void Vector<T>::Reset()
{
  Destroy(Mem_, Size_);
  Allocator_->Free(Mem_);
  Mem_ = Size_ = Capacity_ = nullptr;
}

template <typename T>
inline void Vector<T>::Realloc(i32 const newCapacity)
{
  check(newCapacity > 0, "Invalid capacity");

  i32 const currSize = Size();
  T*        newMem   = Allocator_->Alloc(newCapacity * sizeof(T), alignof(T));
  check(newMem, "Couldn't allocate Vector memory.");

  if (newCapacity >= currSize)
    Algorithm::Move(Mem_, Size_, newMem);
  else
    Algorithm::Move(Mem_, Size_ - (currSize - newCapacity), newMem);

  Destroy(Mem_, Size_);
  Allocator_->Free(Mem_);

  Mem_      = newMem;
  Size_     = Mem_ + currSize;
  Capacity_ = Mem_ + newCapacity;
}

template <typename T>
inline void Vector<T>::Destroy(T* from, T* to)
{
  if constexpr (!std::is_trivially_destructible_v<T>)
  {
    while (from != to)
      from++->~T();
  }
}

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
    Realloc(initialSize);
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
    Realloc(sz);

  T* item = Mem_;
  for (auto&& value : init)
    new (item++) T(std::forward<U>(value));
}

template <typename T>
template <typename U>
inline Vector<T>::Vector(i32 const initialSize, U const& initialValue, IAllocator* allocator)
    : Vector(initialValue, allocator)
{
  static_assert(std::constructible_from<T, decltype(initialValue)>, "Cannot construct T from U.");
  if (initialSize)
    Realloc(initialSize);

  for (T* item = Mem_; item < Size_; ++item)
    new (item) T(initialValue);
}

template <typename T>
template <std::input_iterator Iterator>
inline Vector<T>::Vector(Iterator begin, Iterator end, IAllocator* allocator)
    : Vector(allocator)
{
  constexpr bool CanUseFastPath = std::is_trivially_copyable_v<T> && std::is_same_v<std::remove_cvref_t<Iterator>, T*>;

  i32 initialSize = 0;
  if constexpr (CanUseFastPath)
    initialSize = end - begin;
  else
    initialSize = std::distance(begin, end);

  Realloc(initialSize);
  if constexpr (CanUseFastPath)
  {
    std::memcpy(Mem_, begin, initialSize * sizeof(T));
  }
  else
  {
    for (T* item = Mem_; item < Size_; ++item)
      new (item) T(*begin++);
  }
}

template <typename T>
inline Vector<T>::Vector(Vector const& other)
    : Vector(other, other.Allocator_->IsCopyable() ? other.Allocator_ : globalAllocator)
{
}

template <typename T>
inline Vector<T>::Vector(Vector const& other, IAllocator* allocator)
    : Vector(other.Mem_, other.Size_, allocator)
{
}

template <typename T>
inline Vector<T>::Vector(Vector&& other)
    : Vector()
{
  if (other.Allocator_->IsMovable())
  {
    Mem_       = std::exchange(other.Mem_, nullptr);
    Capacity_  = std::exchange(other.Capacity_, nullptr);
    Size_      = std::exchange(other.Size_, nullptr);
    Allocator_ = std::exchange(other.Allocator_, globalAllocator);
  }
  else
  {
    // TODO: Add logging - Warn about a copy being done instead of a move

    // We still allocate the entire capacity to keep the behavior as similar as possible
    Allocator_ = other.Allocator_->IsCopyable() ? other.Allocator_ : globalAllocator;
    Realloc(other.Capacity());
    Algorithm::Move(other.Mem_, other.Size_, Mem_);
    other.Reset();
  }
}

template <typename T>
inline Vector<T>::~Vector()
{
  Reset();
  if (Allocator_->OwnedByContainer())
    delete Allocator_;
}

template <typename T>
inline Vector<T>& Vector<T>::operator=(Vector const& other)
{
  Destroy(Mem_, Size_);
  Allocator_->Free(Mem_);
  Allocator_ = other.Allocator_->IsCopyable() ? other.Allocator_ : globalAllocator;
  Realloc(other.Capacity_);
  Algorithm::Copy(other.Mem_, other.Size_, Mem_);
  return *this;
}
template <typename T>
inline Vector<T>& Vector<T>::operator=(Vector&& other)
{
  Destroy(Mem_, Size_);
  Allocator_->Free(Mem_);
  if (other.Allocator_->IsMovable())
  {
    Mem_       = std::exchange(other.Mem_, nullptr);
    Capacity_  = std::exchange(other.Capacity_, nullptr);
    Size_      = std::exchange(other.Size_, nullptr);
    Allocator_ = std::exchange(other.Allocator_, globalAllocator);
  }
  else
  {
    // TODO: Add logging - Warn about a copy being done instead of a move
    Allocator_ = other.Allocator_->IsCopyable() ? other.Allocator_ : globalAllocator;
    Realloc(other.Capacity_);
    Algorithm::Move(other.Mem_, other.Size_, Mem_);
    other.Reset();
  }
  return *this;
}
template <typename T>
inline IAllocator* Vector<T>::Allocator() const
{
  return Allocator_;
}

template <typename T>
template <typename U>
inline void Vector<T>::Assign(i32 const newSize, U const& newValue)
{
  static_cast(std::constructible_from<T, decltype(U)>, "Cannot construct Vector<T> from U.");
  if (newSize > Capacity())
    Realloc(Capacity() * ReallocRatio);
  i32 const dsz = newSize <= Size() ? Size() - newSize : 0;
  Destroy(Mem_, Size_ - dsz);
  for (T* item = Mem_; item < Mem_ + newSize; ++item)
    new (item) T(newValue);
  Size_ = Mem_ + newSize;
}

template <typename T>
template <std::input_iterator Iterator>
inline void Vector<T>::Assign(Iterator begin, Iterator end)
{
  static_cast(std::constructible_from<T, decltype(*begin)>, "Cannot construct Vector<T> from the provided iterator.");
}
} // namespace Core
