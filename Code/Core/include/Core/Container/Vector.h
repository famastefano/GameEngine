#pragma once

#include <Core/Algorithm/Algorithm.h>
#include <Core/Allocator/Allocator.h>
#include <Core/Assert/Assert.h>
#include <Core/Definitions.h>
#include <cmath>
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
  i32         Size_;
  i32         Capacity_;

  // Vector's state will now be 1:1 as a default constructed Vector, with the custom allocator.
  void Reset();

  // Reallocates the memory to the specified capacity, possibly without invalidating iterators.
  void Realloc(i32 const newCapacity);

  // Invokes the destructor of all items in the provided range.
  static void Destroy(T* from, T* to);

  // Calculates the new capacity given a size, based on the ReallocRatio.
  static i32 CalculateCapacity(i32 const currCapacity, i32 const desiredSize);

  template <typename U>
  static constexpr bool CanFastInitialize()
  {
    return std::is_trivially_copyable_v<T>
        && std::is_trivially_copyable_v<U>
        && sizeof(T) == sizeof(U)
        && sizeof(T) == sizeof(u8);
  }

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

  // The allocated size, in bytes.
  i32 AllocSize() const;

  void Reserve(i32 const capacity);

  void Resize(i32 const newSize);

  template <typename U = T>
  void Resize(i32 const newSize, U const& value);

  void Swap(Vector& other);

  void Clear();

  template <typename U = T>
  T* Insert(T const* position, U&& value);

  template <typename U = T>
  T* Insert(i32 const position, U&& value)
  {
    return Insert(Data() + position, std::forward<U>(value));
  }

  template <typename U = T>
  T* Insert(T const* position, i32 const count, U const& value);

  template <typename U = T>
  T* Insert(i32 const position, i32 const count, U const& value)
  {
    return Insert(Data() + position, count, value);
  }

  template <std::input_iterator Iterator>
  T* Insert(T const* position, Iterator begin, Iterator end);

  template <std::input_iterator Iterator>
  T* Insert(i32 const position, Iterator begin, Iterator end)
  {
    return Insert(Data() + position, begin, end);
  }

  template <typename... Args>
  T* Emplace(T const* position, Args&&... args);

  template <typename... Args>
  T* Emplace(i32 const position, Args&&... args)
  {
    return Emplace(Data() + position, std::forward<Args>(args)...);
  }

  template <typename... Args>
  T* EmplaceBack(Args&&... args)
  {
    return Emplace(end(), std::forward<Args>(args)...);
  }

  // Fast-construct in-place a new element at the end.
  // WARNING: Doesn't check if there's enough capacity to hold the new element.
  template <typename... Args>
  T* EmplaceBackUnsafe(Args&&... args);

  T* Erase(T* position);
  T* Erase(T* begin, T* end);
  T* Erase(i32 const position);
  T* Erase(i32 const position, i32 const count);

  template <std::predicate<T const&> Comparer>
  T* EraseIf(Comparer&& comparer);

  void PopBack();

  template <typename U = T>
  bool Contains(U const& value) const;

  template <std::predicate<T const&> Comparer>
  bool Contains(Comparer&& comparer) const;

  template <typename U = T>
  T* Find(U const& value);
  template <typename U = T>
  T const* Find(U const& value) const;

  template <std::predicate<T const&> Comparer>
  T* Find(Comparer&& comparer);
  template <std::predicate<T const&> Comparer>
  T const* Find(Comparer&& comparer) const;

  template <typename U = T>
  bool operator==(Vector<U> const& other);

  template <typename U = T>
  bool operator!=(Vector<U> const& other)
  {
    return !operator==(other);
  }
};

template <typename T>
inline void Vector<T>::Reset()
{
  Destroy(begin(), end());
  Allocator_->Free(Mem_, alignof(T));
  Mem_  = nullptr;
  Size_ = Capacity_ = 0;
}

template <typename T>
inline void Vector<T>::Realloc(i32 const newCapacity)
{
  if (newCapacity == 0)
    return;

  i32 const currSize = Size();

  if (Mem_)
  {
    if (T* newMem = (T*)Allocator_->Realloc(Mem_, newCapacity * sizeof(T), alignof(T)))
    {
      Mem_      = newMem;
      Size_     = currSize;
      Capacity_ = newCapacity;
      return;
    }
  }

  T* newMem = (T*)Allocator_->Alloc(newCapacity * sizeof(T), alignof(T));
  check(newMem, "Couldn't allocate Vector memory.");

  if (newCapacity >= currSize)
    Algorithm::Move(begin(), end(), newMem);
  else
    Algorithm::Move(begin(), end() - (currSize - newCapacity), newMem);

  Destroy(begin(), end());
  Allocator_->Free(Mem_, alignof(T));

  Mem_      = newMem;
  Size_     = currSize;
  Capacity_ = newCapacity;
}

template <typename T>
inline void Vector<T>::Destroy(T* from, T* to)
{
  check(from <= to, "Vector Destroy called with invalid range.");
  if constexpr (!std::is_trivially_destructible_v<T>)
  {
    while (from != to)
      from++->~T();
  }
}

template <typename T>
inline i32 Vector<T>::CalculateCapacity(i32 currCapacity, i32 const desiredSize)
{
  if (currCapacity == 0)
    return desiredSize;

  while (currCapacity < desiredSize)
    currCapacity *= (i32)std::ceilf(ReallocRatio);
  return currCapacity;
}

template <typename T>
inline Vector<T>::Vector(IAllocator* allocator)
    : Allocator_(allocator)
    , Mem_(nullptr)
    , Size_(0)
    , Capacity_(0)
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
    Size_ = Capacity_;
    if constexpr (std::is_trivially_default_constructible_v<T>)
    {
      std::memset(Mem_, 0, AllocSize());
    }
    else
    {
      for (T* item = begin(); item < end(); ++item)
        new (item) T;
    }
  }
}

template <typename T>
template <typename U>
inline Vector<T>::Vector(std::initializer_list<U> init, IAllocator* allocator)
    : Vector((i32)init.size(), allocator)
{
  static_assert(std::constructible_from<T, U const&>, "Cannot construct T from U.");
  T* item = Mem_;
  for (auto const& value : init)
    new (item++) T(value);
}

template <typename T>
template <typename U>
inline Vector<T>::Vector(i32 const initialSize, U const& initialValue, IAllocator* allocator)
    : Vector(initialSize, allocator)
{
  static_assert(std::constructible_from<T, decltype(initialValue)>, "Cannot construct T from U.");
  if constexpr (CanFastInitialize<U>())
  {
    std::memset(Mem_, (int)initialValue, AllocSize());
  }
  else
  {
    for (T* item = begin(); item < end(); ++item)
      new (item) T(initialValue);
  }
}

template <typename T>
template <std::input_iterator Iterator>
inline Vector<T>::Vector(Iterator begin, Iterator end, IAllocator* allocator)
    : Vector(allocator)
{
  constexpr bool canUseFastPath = std::is_trivially_copyable_v<T> && std::same_as<std::remove_cvref_t<Iterator>, T*>;

  i32 initialSize = 0;
  if constexpr (canUseFastPath)
    initialSize = i32(end - begin);
  else
    initialSize = i32(std::distance(begin, end));

  Realloc(initialSize);
  Size_ = Capacity_;
  if constexpr (canUseFastPath)
  {
    std::memcpy(Mem_, begin, AllocSize());
  }
  else
  {
    static_assert(std::constructible_from<T, decltype(*begin)>, "Vector(begin, end, allocator) requires T to be constructible from *begin.");
    for (T* item = Mem_; item < Mem_ + Size_; ++item)
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
    : Vector(other.begin(), other.end(), allocator)
{
}

template <typename T>
inline Vector<T>::Vector(Vector&& other)
    : Vector()
{
  if (other.Allocator_->IsMovable())
  {
    Mem_       = std::exchange(other.Mem_, nullptr);
    Capacity_  = std::exchange(other.Capacity_, 0);
    Size_      = std::exchange(other.Size_, 0);
    Allocator_ = std::exchange(other.Allocator_, globalAllocator);
  }
  else
  {
    // TODO: Add logging - Warn about a copy being done instead of a move

    // We still allocate the entire capacity to keep the behavior as similar as possible
    Allocator_ = other.Allocator_->IsCopyable() ? other.Allocator_ : globalAllocator;
    Realloc(other.Capacity());
    Algorithm::Move(other.begin(), other.end(), begin());
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
  Reset();
  Allocator_ = other.Allocator_->IsCopyable() ? other.Allocator_ : globalAllocator;
  Realloc(other.Capacity_);
  Algorithm::Copy(other.begin(), other.end(), begin());
  return *this;
}
template <typename T>
inline Vector<T>& Vector<T>::operator=(Vector&& other)
{
  Reset();
  if (other.Allocator_->IsMovable())
  {
    Mem_       = std::swap(other.Mem_);
    Capacity_  = std::swap(other.Capacity_);
    Size_      = std::swap(other.Size_);
    Allocator_ = std::exchange(other.Allocator_, globalAllocator);
  }
  else
  {
    // TODO: Add logging - Warn about a copy being done instead of a move
    Allocator_ = other.Allocator_->IsCopyable() ? other.Allocator_ : globalAllocator;
    Realloc(other.Capacity_);
    Algorithm::Move(other.begin(), other.end(), begin());
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
  static_assert(std::constructible_from<T, decltype(newValue)>, "Cannot construct Vector<T> from U const&.");
  Clear();

  i32 const currCap = Capacity();
  if (newSize > currCap)
    Realloc(CalculateCapacity(currCap, newSize));

  Size_ = newSize;
  if constexpr (CanFastInitialize<U>())
  {
    std::memset(Mem_, (int)newValue, AllocSize());
  }
  else
  {
    for (T* item = begin(); item < end(); ++item)
      new (item) T(newValue);
  }
}

template <typename T>
template <std::input_iterator Iterator>
inline void Vector<T>::Assign(Iterator begin, Iterator end)
{
  static_assert(std::constructible_from<T, decltype(*begin)>, "Cannot construct Vector<T> from the provided iterator.");
  Clear();

  i32 const newSize = (i32)std::distance(begin, end);
  i32 const currCap = Capacity();
  if (newSize >= currCap)
    Realloc(CalculateCapacity(currCap, newSize));

  Size_ = newSize;

  constexpr bool canUseFastPath = std::is_trivially_copyable_v<T> && std::same_as<std::remove_cvref_t<Iterator>, T*>;
  if constexpr (canUseFastPath)
  {
    std::memcpy(Mem_, begin, AllocSize());
  }
  else
  {
    for (T* item = Mem_; item < Mem_ + Size_; ++item)
      new (item) T(*begin++);
  }
}

template <typename T>
inline T& Vector<T>::operator[](i32 const pos)
{
  check(u32(pos) < u32(Size()), "Vector operator[] out-of-bounds access.");
  return Mem_[pos];
}

template <typename T>
inline T const& Vector<T>::operator[](i32 const pos) const
{
  check(u32(pos) < u32(Size()), "Vector operator[] out-of-bounds access.");
  return Mem_[pos];
}

template <typename T>
inline T& Vector<T>::Front()
{
  check(!IsEmpty(), "Vector is empty, but Front() was called.");
  return Mem_[0];
}

template <typename T>
inline T const& Vector<T>::Front() const
{
  check(!IsEmpty(), "Vector is empty, but Front() was called.");
  return Mem_[0];
}

template <typename T>
inline T& Vector<T>::Back()
{
  check(!IsEmpty(), "Vector is empty, but Back() was called.");
  return *(end() - 1);
}

template <typename T>
inline T const& Vector<T>::Back() const
{
  check(!IsEmpty(), "Vector is empty, but Back() was called.");
  return *(end() - 1);
}

template <typename T>
inline T* Vector<T>::Data()
{
  return Mem_;
}

template <typename T>
inline T const* Vector<T>::Data() const
{
  return Mem_;
}

template <typename T>
inline T* Vector<T>::begin()
{
  return Mem_;
}

template <typename T>
inline T const* Vector<T>::begin() const
{
  return Mem_;
}

template <typename T>
inline T* Vector<T>::end()
{
  return Mem_ + Size_;
}

template <typename T>
inline T const* Vector<T>::end() const
{
  return Mem_ + Size_;
}

template <typename T>
inline bool Vector<T>::IsEmpty() const
{
  return Size_ == 0;
}

template <typename T>
inline i32 Vector<T>::Size() const
{
  return Size_;
}

template <typename T>
inline i32 Vector<T>::Capacity() const
{
  return Capacity_;
}

template <typename T>
inline i32 Vector<T>::AllocSize() const
{
  return Size() * sizeof(T);
}

template <typename T>
inline void Vector<T>::Reserve(i32 const capacity)
{
  i32 const currCap = Capacity();
  if (currCap < capacity)
    Realloc(capacity);
}

template <typename T>
inline void Vector<T>::Resize(i32 const newSize)
{
  static_assert(std::default_initializable<T>, "Vector Resize(newSize) requires T to be default constructible.");

  i32 const currSize = Size();
  i32 const currCap  = Capacity();
  if (currCap < newSize)
    Realloc(newSize);

  if (currSize == 0)
    Size_ = newSize;

  if (currSize < newSize)
  {
    if constexpr (std::is_trivially_default_constructible_v<T>)
    {
      std::memset(begin() + currSize, 0, (end() - (Mem_ + currSize)) * sizeof(T));
    }
    else
    {
      for (T* item = begin() + currSize; item < end(); ++item)
        new (item) T;
    }
  }
  else if (currSize > newSize)
  {
    i32 const ds = currSize - newSize;
    Destroy(begin() + ds, end());
    Size_ = ds;
  }
}

template <typename T>
template <typename U>
inline void Vector<T>::Resize(i32 const newSize, U const& value)
{
  static_assert(std::constructible_from<T, decltype(value)>, "Vector Resize(newSize, value) requires T to be constructible from value.");

  i32 const currSize = Size();
  i32 const currCap  = Capacity();

  if (currSize < newSize)
  {
    if (currCap < newSize)
      Realloc(newSize);

    Size_ = newSize;
    if constexpr (CanFastInitialize<U>())
    {
      std::memset(begin() + currSize, (int)value, (end() - (begin() + currSize)) * sizeof(T));
    }
    else
    {
      for (T* item = begin() + currSize; item < end(); ++item)
        new (item) T(value);
    }
  }
  else if (currSize > newSize)
  {
    i32 const ds = currSize - newSize;
    Destroy(begin() + ds, end());
    Size_ -= ds;
  }
}

template <typename T>
inline void Vector<T>::Swap(Vector& other)
{
  bool const canMove = Allocator_->IsMovable() && other.Allocator_->IsMovable();
  check(canMove, "Cannot swap Vector with immovable allocators, will fallback to a deep-copy without an allocator swap!");
  if (canMove)
  {
    std::swap(Mem_, other.Mem_);
    std::swap(Size_, other.Size_);
    std::swap(Capacity_, other.Capacity_);
    std::swap(Allocator_, other.Allocator_);
  }
  else
  {
    Vector tmp = std::move(other);
    other      = std::move(*this);
    *this      = std::move(tmp);
  }
}

template <typename T>
inline void Vector<T>::Clear()
{
  Destroy(begin(), end());
  Size_ = 0;
}

template <typename T>
template <typename U>
inline T* Vector<T>::Insert(T const* position, U&& value)
{
  return Emplace(position, std::forward<U>(value));
}

template <typename T>
template <typename U>
inline T* Vector<T>::Insert(T const* position, i32 const count, U const& value)
{
  static_assert(std::constructible_from<T, decltype(value)>, "Vector Insert(position, count, value) cannot construct T from value.");
  check(begin() <= position && position <= end(), "Vector Insert(position, count, value) has an invalid position.");
  if (position == end())
  {
    Assign(count, value);
    return begin();
  }

  i32 const posIndex = i32(position - Mem_);
  i32 const currCap  = Capacity();
  i32 const currSize = Size();
  i32 const newSize  = currSize + count;
  if (currCap < newSize)
    Realloc(CalculateCapacity(currCap, newSize));

  T* fromStart = Mem_ + posIndex;
  T* fromEnd   = end();
  T* to        = fromStart + count;
  Algorithm::Move(fromStart, fromEnd, to);
  if constexpr (CanFastInitialize<U>())
  {
    std::memset(Mem_ + posIndex, (int)value, count * sizeof(T));
  }
  else
  {
    for (T* item = Mem_ + posIndex; item < Mem_ + posIndex + count; ++item)
      new (item) T(value);
  }

  Size_ = newSize;
  return Mem_ + posIndex;
}

template <typename T>
template <std::input_iterator Iterator>
inline T* Vector<T>::Insert(T const* position, Iterator begin, Iterator end)
{
  static_assert(std::constructible_from<T, decltype(*begin)>, "Vector Insert(position, begin, end) cannot construct T from *begin.");
  check(Mem_ <= position && position <= Mem_ + Size_, "Vector Insert(position, begin, end) has an invalid position.");
  if (position == this->end())
  {
    Assign(begin, end);
    return this->begin();
  }

  i32 const elemCount = (i32)std::distance(begin, end);
  i32 const posIndex  = i32(position - Mem_);
  i32 const currCap   = Capacity();
  i32 const currSize  = Size();
  i32 const newSize   = currSize + elemCount;
  if (currCap < newSize)
    Realloc(CalculateCapacity(currCap, newSize));

  Algorithm::Move(Mem_ + posIndex, Mem_ + Size_, Mem_ + elemCount);

  Size_ = newSize;

  constexpr bool canUseFastPath = std::is_trivially_copyable_v<T> && std::same_as<std::remove_cvref_t<Iterator>, T*>;
  if constexpr (canUseFastPath)
  {
    std::memcpy(Mem_ + posIndex, begin, elemCount * sizeof(T));
  }
  else
  {
    for (T* item = Mem_ + posIndex; item < Mem_ + posIndex + elemCount; ++item)
      new (item) T(*begin++);
  }
  return Mem_ + posIndex;
}

template <typename T>
template <typename... Args>
inline T* Vector<T>::Emplace(T const* position, Args&&... args)
{
  static_assert(std::constructible_from<T, Args...>, "Vector Emplace(position, Args...) cannot construct T from Args.");
  check(begin() <= position && position <= end(), "Vector Emplace(position, Args...) has an invalid position.");
  i32 const posIndex = i32(position - Mem_);
  i32 const size     = Size();
  i32 const cap      = Capacity();
  if (size == cap)
    Realloc(CalculateCapacity(cap, size + 1));

  T* pos = Mem_ + posIndex;
  Algorithm::Move(pos, end(), pos + 1);
  new (pos) T(std::forward<Args>(args)...);
  ++Size_;
  return pos;
}

template <typename T>
template <typename... Args>
inline T* Vector<T>::EmplaceBackUnsafe(Args&&... args)
{
  check(Size() < Capacity(), "Vector EmplaceBackUnsafe(Args) out of bounds insertion.");
  T* pos = Mem_ + Size_++;
  new (pos) T(std::forward<Args>(args)...);
  return pos;
}

template <typename T>
inline T* Vector<T>::Erase(T* position)
{
  check(begin() <= position && position <= end(), "Vector Erase(position) has an invalid position.");
  if (IsEmpty() || position == end())
    return end();
  return Erase(position, position + 1);
}

template <typename T>
inline T* Vector<T>::Erase(T* begin, T* end)
{
  check(begin <= end && Mem_ <= begin && end <= Mem_ + Size_, "Vector Erase(begin, end) has an invalid range.");
  if (IsEmpty() || begin == end)
    return Mem_ + Size_;

  if constexpr (!std::is_trivially_destructible_v<T>)
  {
    for (T* item = begin; item < end; ++item)
    {
      T&& tmp{std::move(*item)};
      tmp.~T();
    }
  }
  // It's safe to move as the range [begin, end) has moved-from objects
  Algorithm::Move(end, Mem_ + Size_, begin);
  Size_ -= i32(end - begin);

  if (Mem_ + Size_ <= end)
    return Mem_ + Size_;
  return end - 1;
}

template <typename T>
inline T* Vector<T>::Erase(i32 const position)
{
  return Erase(Mem_ + position);
}

template <typename T>
inline T* Vector<T>::Erase(i32 const position, i32 const count)
{
  return Erase(Mem_ + position, Mem_ + position + count);
}

template <typename T>
template <std::predicate<T const&> Comparer>
T* Vector<T>::EraseIf(Comparer&& comparer)
{
  T* curr = begin();
  while (curr != end())
    curr = comparer(*curr) ? Erase(curr) : curr + 1;
  return curr;
}

template <typename T>
inline void Vector<T>::PopBack()
{
  if (!IsEmpty())
    Mem_[--Size_].~T();
}

template <typename T>
template <typename U>
inline bool Vector<T>::Contains(U const& value) const
{
  return Find(value) != end();
}

template <typename T>
template <std::predicate<T const&> Comparer>
inline bool Vector<T>::Contains(Comparer&& comparer) const
{
  return Find(std::forward<Comparer>(comparer)) != end();
}

template <typename T>
template <typename U>
inline T* Vector<T>::Find(U const& value)
{
  auto const* selfConst = this;
  return const_cast<T*>(selfConst->Find(value));
}

template <typename T>
template <typename U>
inline T const* Vector<T>::Find(U const& value) const
{
  static_assert(std::equality_comparable_with<decltype(*begin()), decltype(value)>, "Vector Find(value) cannot compare T == U");
  for (auto const& item : *this)
  {
    if (item == value)
      return &item;
  }
  return end();
}

template <typename T>
template <std::predicate<T const&> Comparer>
inline T* Vector<T>::Find(Comparer&& comparer)
{
  auto const* selfConst = this;
  return const_cast<T*>(selfConst->Find(std::forward<Comparer>(comparer)));
}

template <typename T>
template <std::predicate<T const&> Comparer>
inline T const* Vector<T>::Find(Comparer&& comparer) const
{
  for (auto const& item : *this)
  {
    if (comparer(item))
      return &item;
  }
  return end();
}

template <typename T>
template <typename U>
inline bool Vector<T>::operator==(Vector<U> const& other)
{
  static_assert(std::equality_comparable_with<T const&, U const&>, "Vector operator==(Vector<U>) cannot compare T == U");
  if (Size() != other.Size())
    return false;

  if constexpr (std::is_trivially_copyable_v<T> && std::is_trivially_copyable_v<U> && sizeof(T) == sizeof(U))
  {
    return std::memcmp(Mem_, other.Data(), AllocSize()) == 0;
  }
  else
  {
    for (i32 i = 0; i < Size(); ++i)
    {
      if (!((*this)[i] == other[i]))
        return false;
    }
    return true;
  }
}
} // namespace Core
