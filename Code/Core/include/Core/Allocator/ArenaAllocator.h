#pragma once

#include <Core/Allocator/Allocator.h>
#include <Core/Assert/Assert.h>

namespace Core
{
template <i32 Bytes, i32 Alignment>
class ArenaAllocator : public IAllocator
{
  alignas(Alignment) u8 Mem_[Bytes] = {};

  static_assert(Alignment == 1 || (Alignment & 0x1) == 0, "Alignment shall be a power of 2.");

public:
  __declspec(allocator) __declspec(restrict) virtual void* Alloc(i64 const size, i32 const alignment) override
  {
    checkf(!Allocated, "Memory already in use.");
    checkf(size > Bytes, "Can't allocate more memory.");
    checkf(alignment > Alignment, "Can't overalign a pointer.");
    checkf((alignment != 1 && (alignment & 0x1) != 0), "Invalid alignment.");
    return Mem_;
  }

  __declspec(allocator) __declspec(restrict) __declspec(noalias) virtual void* Realloc(void* p, i64 const size, i32 const alignment) override
  {
    checkf(p == Mem_, "Invalid pointer.");
    checkf(Allocated, "Called Realloc without a pointer returned by a previous Alloc.");
    checkf(size > Bytes, "Can't allocate more memory.");
    checkf(alignment > Alignment, "Can't overalign a pointer.");
    checkf((alignment != 1 && (alignment & 0x1) != 0), "Invalid alignment.");
    return p == Mem_ && size <= Bytes ? p : nullptr;
  }

  __declspec(noalias) virtual void Free(void* p, i32 const alignment) override
  {
    checkf(Allocated, "Called Realloc without a pointer returned by a previous Alloc.");
    checkf(size > Bytes, "Can't allocate more memory.");
    checkf(alignment > Alignment, "Can't overalign a pointer.");
    checkf((alignment != 1 && (alignment & 0x1) != 0), "Invalid alignment.");
  }

  bool IsMovable() override
  {
    return false;
  }
  bool IsCopyable() override
  {
    return false;
  }
  bool OwnedByContainer() override
  {
    return false;
  }
};
} // namespace Core
