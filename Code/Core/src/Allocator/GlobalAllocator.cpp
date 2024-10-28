#include <Core/Allocator/GlobalAllocator.h>
#include <Core/Assert/Assert.h>
#include <Windows.h>
#include <new>

static Core::GlobalAllocator globalAllocatorInstance;

namespace Core
{
IAllocator* globalAllocator = &globalAllocatorInstance;

static HANDLE MemHandle = GetProcessHeap() ? GetProcessHeap() : HeapCreate(0, 0, 0);

constexpr void* ToAlignedPointer(void* p, u64 const alignment)
{
  u64 addr  = (u64)p;
  addr     += sizeof(void*) + alignment - 1;
  addr     &= ~(alignment - 1);
  return (void*)addr;
}

constexpr void* FromAlignedPointer(void* p, i32 const alignment)
{
  return alignment <= MEMORY_ALLOCATION_ALIGNMENT ? p : ((void**)p)[-1];
}

void* GlobalAllocator::Alloc(i64 size, i32 const alignment)
{
  check((alignment == 1 || !(alignment & 0x1)), "Alignment must be a power of 2.");
  if (alignment <= MEMORY_ALLOCATION_ALIGNMENT)
    return HeapAlloc(MemHandle, HEAP_ZERO_MEMORY, size);

  i32 const   offset    = sizeof(void*) + (alignment - 1);
  void* const allocated = HeapAlloc(MemHandle, HEAP_ZERO_MEMORY, size + offset);
  if (!allocated)
    return nullptr;

  void* aligned         = ToAlignedPointer(allocated, alignment);
  ((void**)aligned)[-1] = allocated;
  return aligned;
}

void* GlobalAllocator::Realloc(void* toRealloc, i64 size, i32 const alignment)
{
  check((alignment == 1 || !(alignment & 0x1)), "Alignment must be a power of 2.");
  if (alignment <= MEMORY_ALLOCATION_ALIGNMENT)
    return HeapReAlloc(MemHandle, HEAP_REALLOC_IN_PLACE_ONLY | HEAP_ZERO_MEMORY, toRealloc, size);

  void*     actualPtr   = FromAlignedPointer(toRealloc, alignment);
  i32 const offset      = sizeof(void*) + (alignment - 1);
  void*     reallocated = HeapReAlloc(MemHandle, HEAP_REALLOC_IN_PLACE_ONLY | HEAP_ZERO_MEMORY, actualPtr, size + offset);
  if (!reallocated)
    return nullptr;

  void* aligned         = ToAlignedPointer(reallocated, alignment);
  ((void**)aligned)[-1] = reallocated;
  return aligned;
}

void GlobalAllocator::Free(void* p, i32 const alignment)
{
  if (p)
  {
    void* actual = FromAlignedPointer(p, alignment);
    HeapFree(MemHandle, 0, actual);
  }
}

bool GlobalAllocator::IsMovable()
{
  return true;
}

bool GlobalAllocator::IsCopyable()
{
  return true;
}

bool GlobalAllocator::OwnedByContainer()
{
  return false;
}
} // namespace Core

#pragma warning(disable : 28'251)
// operator new

void* operator new(std::size_t count)
{
  return globalAllocatorInstance.Alloc(count, MEMORY_ALLOCATION_ALIGNMENT);
}
void* operator new[](std::size_t count)
{
  return globalAllocatorInstance.Alloc(count, MEMORY_ALLOCATION_ALIGNMENT);
}
void* operator new(std::size_t count, std::align_val_t al)
{
  return globalAllocatorInstance.Alloc(count, (i32)al);
}
void* operator new[](std::size_t count, std::align_val_t al)
{
  return globalAllocatorInstance.Alloc(count, (i32)al);
}
void* operator new(std::size_t count, std::nothrow_t const&) noexcept
{
  return globalAllocatorInstance.Alloc(count, MEMORY_ALLOCATION_ALIGNMENT);
}
void* operator new[](std::size_t count, std::nothrow_t const&) noexcept
{
  return globalAllocatorInstance.Alloc(count, MEMORY_ALLOCATION_ALIGNMENT);
}
void* operator new(std::size_t count, std::align_val_t al, std::nothrow_t const&) noexcept
{
  return globalAllocatorInstance.Alloc(count, (i32)al);
}
void* operator new[](std::size_t count, std::align_val_t al, std::nothrow_t const&) noexcept
{
  return globalAllocatorInstance.Alloc(count, (i32)al);
}

// operator delete

void operator delete(void* ptr) noexcept
{
  globalAllocatorInstance.Free(ptr, MEMORY_ALLOCATION_ALIGNMENT);
}
void operator delete[](void* ptr) noexcept
{
  globalAllocatorInstance.Free(ptr, MEMORY_ALLOCATION_ALIGNMENT);
}
void operator delete(void* ptr, std::align_val_t al) noexcept
{
  globalAllocatorInstance.Free(ptr, (i32)al);
}
void operator delete[](void* ptr, std::align_val_t al) noexcept
{
  globalAllocatorInstance.Free(ptr, (i32)al);
}
void operator delete(void* ptr, std::size_t) noexcept
{
  globalAllocatorInstance.Free(ptr, MEMORY_ALLOCATION_ALIGNMENT);
}
void operator delete[](void* ptr, std::size_t) noexcept
{
  globalAllocatorInstance.Free(ptr, MEMORY_ALLOCATION_ALIGNMENT);
}
void operator delete(void* ptr, std::size_t, std::align_val_t al) noexcept
{
  globalAllocatorInstance.Free(ptr, (i32)al);
}
void operator delete[](void* ptr, std::size_t, std::align_val_t al) noexcept
{
  globalAllocatorInstance.Free(ptr, (i32)al);
}
void operator delete(void* ptr, std::nothrow_t const&) noexcept
{
  globalAllocatorInstance.Free(ptr, MEMORY_ALLOCATION_ALIGNMENT);
}
void operator delete[](void* ptr, std::nothrow_t const&) noexcept
{
  globalAllocatorInstance.Free(ptr, MEMORY_ALLOCATION_ALIGNMENT);
}
void operator delete(void* ptr, std::align_val_t al, std::nothrow_t const&) noexcept
{
  globalAllocatorInstance.Free(ptr, (i32)al);
}
void operator delete[](void* ptr, std::align_val_t al, std::nothrow_t const&) noexcept
{
  globalAllocatorInstance.Free(ptr, (i32)al);
}
#pragma warning(default : 28'251)
