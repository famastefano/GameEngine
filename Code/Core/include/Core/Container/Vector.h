#pragma once

#include <Core/Algorithm/Algorithm.h>
#include <Core/Allocator/Allocator.h>
#include <Core/Assert/Assert.h>
#include <Core/Definitions.h>
#include <Core/Iterator/ContiguousIterator.h>
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
  constexpr void Reset();

  // Reallocates the memory to the specified capacity, possibly without invalidating iterators.
  constexpr void Realloc(i32 const newCapacity);

  // Invokes the destructor of all items in the provided range.
  constexpr static void Destroy(T* from, T* to);

  // Calculates the new capacity given a size, based on the ReallocRatio.
  constexpr static i32 CalculateCapacity(i32 const currCapacity, i32 const desiredSize);

  template <typename U>
  static constexpr bool CanFastInitialize()
  {
    return std::is_trivially_copyable_v<T>
        && std::is_trivially_copyable_v<U>
        && sizeof(T) == sizeof(U)
        && sizeof(T) == sizeof(u8);
  }

public:
  constexpr Vector(IAllocator* allocator = globalAllocator);
  constexpr Vector(i32 const initialSize, IAllocator* allocator = globalAllocator);

  template <typename U = T>
  constexpr Vector(std::initializer_list<U> init, IAllocator* allocator = globalAllocator);

  template <typename U = T>
  constexpr Vector(i32 const initialSize, U const& initialValue, IAllocator* allocator = globalAllocator);

  template <std::input_iterator Iterator>
  constexpr Vector(Iterator begin, Iterator end, IAllocator* allocator = globalAllocator);

  constexpr Vector(Vector const& other);
  constexpr Vector(Vector const& other, IAllocator* allocator);

  constexpr Vector(Vector&& other);

  constexpr ~Vector();

  constexpr Vector& operator=(Vector const& other);
  constexpr Vector& operator=(Vector&& other);

  constexpr IAllocator* Allocator() const;

  template <typename U = T>
  constexpr void Assign(i32 const newSize, U const& newValue);

  template <std::input_iterator Iterator>
  constexpr void Assign(Iterator begin, Iterator end);

  constexpr T&       operator[](i32 const pos);
  constexpr T const& operator[](i32 const pos) const;

  constexpr T&       Front();
  constexpr T const& Front() const;

  constexpr T&       Back();
  constexpr T const& Back() const;

  constexpr T*       Data();
  constexpr T const* Data() const;

  constexpr T*       begin();
  constexpr T const* begin() const;

  constexpr T*       end();
  constexpr T const* end() const;

  constexpr ReverseContiguousIterator<T>       rbegin();
  constexpr ReverseContiguousIterator<T const> rbegin() const;

  constexpr ReverseContiguousIterator<T>       rend();
  constexpr ReverseContiguousIterator<T const> rend() const;

  constexpr bool IsEmpty() const;

  constexpr i32 Size() const;

  constexpr i32 Capacity() const;

  // The allocated size, in bytes.
  constexpr i32 AllocSize() const;

  constexpr void Reserve(i32 const capacity);

  constexpr void Resize(i32 const newSize);

  template <typename U = T>
  constexpr void Resize(i32 const newSize, U const& value);

  constexpr void Swap(Vector& other);

  constexpr void Clear();

  template <typename U = T>
  constexpr T* Insert(T const* position, U&& value);

  template <typename U = T>
  constexpr T* Insert(i32 const position, U&& value)
  {
    return Insert(Data() + position, std::forward<U>(value));
  }

  template <typename U = T>
  constexpr T* Insert(T const* position, i32 const count, U const& value);

  template <typename U = T>
  constexpr T* Insert(i32 const position, i32 const count, U const& value)
  {
    return Insert(Data() + position, count, value);
  }

  template <std::input_iterator Iterator>
  constexpr T* Insert(T const* position, Iterator begin, Iterator end);

  template <std::input_iterator Iterator>
  constexpr T* Insert(i32 const position, Iterator begin, Iterator end)
  {
    return Insert(Data() + position, begin, end);
  }

  template <typename... Args>
  constexpr T* Emplace(T const* position, Args&&... args);

  template <typename... Args>
  constexpr T* Emplace(i32 const position, Args&&... args)
  {
    return Emplace(Data() + position, std::forward<Args>(args)...);
  }

  template <typename... Args>
  constexpr T* EmplaceBack(Args&&... args)
  {
    return Emplace(end(), std::forward<Args>(args)...);
  }

  // Fast-construct in-place a new element at the end.
  // WARNING: Doesn't check if there's enough capacity to hold the new element.
  template <typename... Args>
  constexpr T* EmplaceBackUnsafe(Args&&... args);

  constexpr T* Erase(T* position);
  constexpr T* Erase(T* begin, T* end);
  constexpr T* Erase(i32 const position);
  constexpr T* Erase(i32 const position, i32 const count);

  template <std::predicate<T const&> Comparer>
  constexpr T* EraseIf(Comparer&& comparer);

  constexpr void PopBack();

  template <typename U = T>
  constexpr bool Contains(U const& value) const;

  template <std::predicate<T const&> Comparer>
  constexpr bool Contains(Comparer&& comparer) const;

  template <typename U = T>
  constexpr T* Find(U const& value);
  template <typename U = T>
  constexpr T const* Find(U const& value) const;

  template <std::predicate<T const&> Comparer>
  constexpr T* Find(Comparer&& comparer);
  template <std::predicate<T const&> Comparer>
  constexpr T const* Find(Comparer&& comparer) const;

  template <typename U = T>
  constexpr bool operator==(Vector<U> const& other);

  template <typename U = T>
  constexpr bool operator!=(Vector<U> const& other)
  {
    return !operator==(other);
  }
};

template <typename T>
constexpr inline void Vector<T>::Reset()
{
  Destroy(begin(), end());
  Allocator_->Free(Mem_, alignof(T));
  Mem_  = nullptr;
  Size_ = Capacity_ = 0;
}

template <typename T>
constexpr inline void Vector<T>::Realloc(i32 const newCapacity)
{
  if (newCapacity == 0)
    return;

  i32 const currSize = Size();

  if (Mem_)
  {
    if (T* newMem = (T*)Allocator_->Realloc(Mem_, newCapacity * (i64)sizeof(T), alignof(T)))
    {
      Mem_      = newMem;
      Size_     = currSize;
      Capacity_ = newCapacity;
      return;
    }
  }

  T* newMem = (T*)Allocator_->Alloc(newCapacity * (i64)sizeof(T), alignof(T));
  checkf(newMem, "Couldn't allocate Vector memory.");

  if constexpr (!std::is_trivially_default_constructible_v<T>)
  {
    for (T* item = newMem; item < newMem + currSize; ++item)
      new (item) T();
  }

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
constexpr inline void Vector<T>::Destroy(T* from, T* to)
{
  checkf(from <= to, "Vector Destroy called with invalid range.");
  if constexpr (!std::is_trivially_destructible_v<T>)
  {
    while (from != to)
      from++->~T();
  }
}

template <typename T>
constexpr inline i32 Vector<T>::CalculateCapacity(i32 currCapacity, i32 const desiredSize)
{
  if (currCapacity == 0)
    return desiredSize;

  while (currCapacity < desiredSize)
    currCapacity *= (i32)std::ceilf(ReallocRatio);
  return currCapacity;
}

template <typename T>
constexpr inline Vector<T>::Vector(IAllocator* allocator)
    : Allocator_(allocator)
    , Mem_(nullptr)
    , Size_(0)
    , Capacity_(0)
{
  checkf(allocator, "Invalid allocator!");
}

template <typename T>
constexpr inline Vector<T>::Vector(i32 const initialSize, IAllocator* allocator)
    : Vector(allocator)
{
  static_assert(std::default_initializable<T>, "T isn't default constructible.");
  if (initialSize)
  {
    Realloc(initialSize);
    Size_ = Capacity_;
    if constexpr (std::is_trivially_default_constructible_v<T>)
    {
      std::memset(Mem_, 0, (u64)AllocSize());
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
constexpr inline Vector<T>::Vector(std::initializer_list<U> init, IAllocator* allocator)
    : Vector(init.begin(), init.end(), allocator)
{
}

template <typename T>
template <typename U>
constexpr inline Vector<T>::Vector(i32 const initialSize, U const& initialValue, IAllocator* allocator)
    : Vector(initialSize, allocator)
{
  Assign(initialSize, initialValue);
}

template <typename T>
template <std::input_iterator Iterator>
constexpr inline Vector<T>::Vector(Iterator begin, Iterator end, IAllocator* allocator)
    : Vector(allocator)
{
  Assign(begin, end);
}

template <typename T>
constexpr inline Vector<T>::Vector(Vector const& other)
    : Vector(other, other.Allocator_->IsCopyable() ? other.Allocator_ : globalAllocator)
{
}

template <typename T>
constexpr inline Vector<T>::Vector(Vector const& other, IAllocator* allocator)
    : Vector(other.begin(), other.end(), allocator)
{
}

template <typename T>
constexpr inline Vector<T>::Vector(Vector&& other)
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
    // We still allocate the entire capacity to keep the behavior as similar as possible
    Allocator_ = other.Allocator_->IsCopyable() ? other.Allocator_ : globalAllocator;
    Realloc(other.Capacity());
    Algorithm::Move(other.begin(), other.end(), begin());
    other.Reset();
  }
}

template <typename T>
constexpr inline Vector<T>::~Vector()
{
  Reset();
  if (Allocator_->OwnedByContainer())
    delete Allocator_;
}

template <typename T>
constexpr inline Vector<T>& Vector<T>::operator=(Vector const& other)
{
  Reset();
  Allocator_ = other.Allocator_->IsCopyable() ? other.Allocator_ : globalAllocator;
  Realloc(other.Capacity_);
  Algorithm::Copy(other.begin(), other.end(), begin());
  return *this;
}
template <typename T>
constexpr inline Vector<T>& Vector<T>::operator=(Vector&& other)
{
  Reset();
  if (other.Allocator_->IsMovable())
  {
    Mem_       = std::exchange(other.Mem_, nullptr);
    Capacity_  = std::exchange(other.Capacity_, 0);
    Size_      = std::exchange(other.Size_, 0);
    Allocator_ = std::exchange(other.Allocator_, globalAllocator);
  }
  else
  {
    Allocator_ = other.Allocator_->IsCopyable() ? other.Allocator_ : globalAllocator;
    Realloc(other.Capacity_);
    Algorithm::Move(other.begin(), other.end(), begin());
    other.Reset();
  }
  return *this;
}

template <typename T>
constexpr inline IAllocator* Vector<T>::Allocator() const
{
  return Allocator_;
}

template <typename T>
template <typename U>
constexpr inline void Vector<T>::Assign(i32 const newSize, U const& newValue)
{
  static_assert(std::constructible_from<T, decltype(newValue)>, "Cannot construct Vector<T> from U const&.");
  Clear();

  i32 const currCap = Capacity();
  if (newSize > currCap)
    Realloc(CalculateCapacity(currCap, newSize));

  Size_ = newSize;
  if constexpr (CanFastInitialize<U>())
  {
    std::memset(Mem_, (int)newValue, (u64)AllocSize());
  }
  else
  {
    for (T* item = begin(); item < end(); ++item)
#pragma warning(suppress : 4'365) // 'initializing': conversion from '' to '', signed/unsigned mismatch
      new (item) T(newValue);
  }
}

template <typename T>
template <std::input_iterator Iterator>
constexpr inline void Vector<T>::Assign(Iterator begin, Iterator end)
{
  static_assert(std::constructible_from<T, decltype(*begin)>, "Cannot construct Vector<T> from the provided iterator.");
  Clear();

  i32 const newSize = (i32)std::distance(begin, end);
  i32 const currCap = Capacity();
  if (newSize > currCap)
    Realloc(CalculateCapacity(currCap, newSize));

  Size_ = newSize;

  constexpr bool canUseFastPath = std::is_trivially_copyable_v<T> && std::same_as<std::remove_cvref_t<Iterator>, T*>;
  if constexpr (canUseFastPath)
  {
    std::memcpy(Mem_, begin, (u64)AllocSize());
  }
  else
  {
    for (T* item = Mem_; item < Mem_ + Size_; ++item)
#pragma warning(suppress : 4'365) // 'initializing': conversion from '' to '', signed/unsigned mismatch
      new (item) T(*begin++);
  }
}

template <typename T>
constexpr inline T& Vector<T>::operator[](i32 const pos)
{
  checkf(u32(pos) < u32(Size()), "Vector operator[] out-of-bounds access.");
  return Mem_[pos];
}

template <typename T>
constexpr inline T const& Vector<T>::operator[](i32 const pos) const
{
  checkf(u32(pos) < u32(Size()), "Vector operator[] out-of-bounds access.");
  return Mem_[pos];
}

template <typename T>
constexpr inline T& Vector<T>::Front()
{
  checkf(!IsEmpty(), "Vector is empty, but Front() was called.");
  return Mem_[0];
}

template <typename T>
constexpr inline T const& Vector<T>::Front() const
{
  checkf(!IsEmpty(), "Vector is empty, but Front() was called.");
  return Mem_[0];
}

template <typename T>
constexpr inline T& Vector<T>::Back()
{
  checkf(!IsEmpty(), "Vector is empty, but Back() was called.");
  return *(end() - 1);
}

template <typename T>
constexpr inline T const& Vector<T>::Back() const
{
  checkf(!IsEmpty(), "Vector is empty, but Back() was called.");
  return *(end() - 1);
}

template <typename T>
constexpr inline T* Vector<T>::Data()
{
  return Mem_;
}

template <typename T>
constexpr inline T const* Vector<T>::Data() const
{
  return Mem_;
}

template <typename T>
constexpr inline T* Vector<T>::begin()
{
  return Mem_;
}

template <typename T>
constexpr inline T const* Vector<T>::begin() const
{
  return Mem_;
}

template <typename T>
constexpr inline T* Vector<T>::end()
{
  return Mem_ + Size_;
}

template <typename T>
constexpr inline T const* Vector<T>::end() const
{
  return Mem_ + Size_;
}

template <typename T>
inline constexpr ReverseContiguousIterator<T> Vector<T>::rbegin()
{
  using It = ReverseContiguousIterator<T>;
  return IsEmpty() ? It(nullptr) : It(end() - 1);
}

template <typename T>
inline constexpr ReverseContiguousIterator<T const> Vector<T>::rbegin() const
{
  using It = ReverseContiguousIterator<T const>;
  return IsEmpty() ? It(nullptr) : It(end() - 1);
}
template <typename T>
inline constexpr ReverseContiguousIterator<T> Vector<T>::rend()
{
  using It = ReverseContiguousIterator<T>;
  return IsEmpty() ? It(nullptr) : It(begin() - 1);
}

template <typename T>
inline constexpr ReverseContiguousIterator<T const> Vector<T>::rend() const
{
  using It = ReverseContiguousIterator<T const>;
  return IsEmpty() ? It(nullptr) : It(begin() - 1);
}

template <typename T>
constexpr inline bool Vector<T>::IsEmpty() const
{
  return Size_ == 0;
}

template <typename T>
constexpr inline i32 Vector<T>::Size() const
{
  return Size_;
}

template <typename T>
constexpr inline i32 Vector<T>::Capacity() const
{
  return Capacity_;
}

template <typename T>
constexpr inline i32 Vector<T>::AllocSize() const
{
  return Size() * (i32)sizeof(T);
}

template <typename T>
constexpr inline void Vector<T>::Reserve(i32 const capacity)
{
  i32 const currCap = Capacity();
  if (currCap < capacity)
    Realloc(capacity);
}

template <typename T>
constexpr inline void Vector<T>::Resize(i32 const newSize)
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
constexpr inline void Vector<T>::Resize(i32 const newSize, U const& value)
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
constexpr inline void Vector<T>::Swap(Vector& other)
{
  if (verifyf(Allocator_->IsMovable() && other.Allocator_->IsMovable(), "Cannot swap Vector with immovable allocators, will fallback to a deep-copy without an allocator swap!"))
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
constexpr inline void Vector<T>::Clear()
{
  Destroy(begin(), end());
  Size_ = 0;
}

template <typename T>
template <typename U>
constexpr inline T* Vector<T>::Insert(T const* position, U&& value)
{
  return Emplace(position, std::forward<U>(value));
}

template <typename T>
template <typename U>
constexpr inline T* Vector<T>::Insert(T const* position, i32 const count, U const& value)
{
  static_assert(std::constructible_from<T, decltype(value)>, "Vector Insert(position, count, value) cannot construct T from value.");
  checkf(begin() <= position && position <= end(), "Vector Insert(position, count, value) has an invalid position.");
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
constexpr inline T* Vector<T>::Insert(T const* position, Iterator begin, Iterator end)
{
  static_assert(std::constructible_from<T, decltype(*begin)>, "Vector Insert(position, begin, end) cannot construct T from *begin.");
  checkf(Mem_ <= position && position <= Mem_ + Size_, "Vector Insert(position, begin, end) has an invalid position.");

  i32 const elemCount = (i32)std::distance(begin, end);
  i32 const posIndex  = i32(position - Mem_);
  i32 const currCap   = Capacity();
  i32 const currSize  = Size();
  i32 const newSize   = currSize + elemCount;
  if (currCap < newSize)
    Realloc(CalculateCapacity(currCap, newSize));

  Algorithm::Move(Mem_ + posIndex, Mem_ + Size_, Mem_ + posIndex + elemCount);

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
constexpr inline T* Vector<T>::Emplace(T const* position, Args&&... args)
{
  static_assert(std::constructible_from<T, Args...>, "Vector Emplace(position, Args...) cannot construct T from Args.");
  checkf(begin() <= position && position <= end(), "Vector Emplace(position, Args...) has an invalid position.");
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
constexpr inline T* Vector<T>::EmplaceBackUnsafe(Args&&... args)
{
  checkf(Size() < Capacity(), "Vector EmplaceBackUnsafe(Args) out of bounds insertion.");
  T* pos = Mem_ + Size_++;
  new (pos) T(std::forward<Args>(args)...);
  return pos;
}

template <typename T>
constexpr inline T* Vector<T>::Erase(T* position)
{
  checkf(!position || begin() <= position && position <= end(), "Vector Erase(position) has an invalid position.");
  if (IsEmpty() || position == end())
    return end();
  return Erase(position, position + 1);
}

template <typename T>
constexpr inline T* Vector<T>::Erase(T* begin, T* end)
{
  checkf(begin <= end && Mem_ <= begin && end <= Mem_ + Size_, "Vector Erase(begin, end) has an invalid range.");
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
constexpr inline T* Vector<T>::Erase(i32 const position)
{
  return Erase(Mem_ + position);
}

template <typename T>
constexpr inline T* Vector<T>::Erase(i32 const position, i32 const count)
{
  return Erase(Mem_ + position, Mem_ + position + count);
}

template <typename T>
template <std::predicate<T const&> Comparer>
constexpr T* Vector<T>::EraseIf(Comparer&& comparer)
{
  T* curr = begin();
  while (curr != end())
    curr = comparer(*curr) ? Erase(curr) : curr + 1;
  return curr;
}

template <typename T>
constexpr inline void Vector<T>::PopBack()
{
  if (!IsEmpty())
    Mem_[--Size_].~T();
}

template <typename T>
template <typename U>
constexpr inline bool Vector<T>::Contains(U const& value) const
{
  return Find(value) != end();
}

template <typename T>
template <std::predicate<T const&> Comparer>
constexpr inline bool Vector<T>::Contains(Comparer&& comparer) const
{
  return Find(std::forward<Comparer>(comparer)) != end();
}

template <typename T>
template <typename U>
constexpr inline T* Vector<T>::Find(U const& value)
{
  auto const* selfConst = this;
  return const_cast<T*>(selfConst->Find(value));
}

template <typename T>
template <typename U>
constexpr inline T const* Vector<T>::Find(U const& value) const
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
constexpr inline T* Vector<T>::Find(Comparer&& comparer)
{
  auto const* selfConst = this;
  return const_cast<T*>(selfConst->Find(std::forward<Comparer>(comparer)));
}

template <typename T>
template <std::predicate<T const&> Comparer>
constexpr inline T const* Vector<T>::Find(Comparer&& comparer) const
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
constexpr inline bool Vector<T>::operator==(Vector<U> const& other)
{
  static_assert(std::equality_comparable_with<T const&, U const&>, "Vector operator==(Vector<U>) cannot compare T == U");
  if (Size() != other.Size())
    return false;

  if constexpr (std::is_trivially_copyable_v<T> && std::is_trivially_copyable_v<U> && sizeof(T) == sizeof(U))
  {
    return std::memcmp(Mem_, other.Data(), (u64)AllocSize()) == 0;
  }
  else
  {
    for (i32 i = 0; i < Size(); ++i)
    {
#pragma warning(suppress : 4'388) // '==': signed/unsigned mismatch
      if (!((*this)[i] == other[i]))
        return false;
    }
    return true;
  }
}
} // namespace Core
