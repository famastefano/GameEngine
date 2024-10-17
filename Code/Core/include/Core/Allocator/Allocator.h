#pragma once

#include <Core/API.h>
#include <Core/Definitions.h>

namespace Core
{
// Interface used by all the containers in the std.
// Instead of being type-intrusive, as it literally changes the container type,
// we pay the price of storing 1 pointer to an allocator
class CORE_API IAllocator
{
public:
  virtual ~IAllocator() = default;

  // Allocates a contiguous block of memory with at least `size` capacity.
  // Returns:
  //   success, a valid pointer
  //   fail, nullptr
  __declspec(allocator) __declspec(restrict) virtual void* Alloc(i64 const size, i32 const alignment) = 0;

  // Reallocates a contiguous block of memory with at least `size` capacity.
  // Returns:
  //   success, a valid pointer
  //   fail, nullptr
  __declspec(allocator) __declspec(restrict) __declspec(noalias) virtual void* Realloc(void* p, i64 const size, i32 const alignment) = 0;

  // Frees the block of memory previously allocated via a call to Alloc()
  // WARNING: `p` shall be the exact same pointer returned by Alloc()
  __declspec(noalias) virtual void Free(void* p) = 0;

  // If the allocator is also "moved into" the new container when a move operation is performed.
  // WARNING: if this is false, a copy will be made.
  virtual bool IsMovable() = 0;

  // If the allocator can be reused when copying a container, otherwise it fallbacks to the global allocator.
  virtual bool IsCopyable() = 0;

  // The container this allocator is passed to, will automatically destroy it
  // depending on the operations performed on the container itself (destructor, changing allocator etc.)
  virtual bool OwnedByContainer() = 0;
};

extern CORE_API IAllocator* globalAllocator;
} // namespace Core
