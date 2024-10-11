#include <Windows.h>
#include <new>
#include <Core/Allocator/GlobalAllocator.h>
#include <Core/Assert/Assert.h>

static Core::GlobalAllocator globalAllocatorInstance;

namespace Core
{
IAllocator* globalAllocator = &globalAllocatorInstance;

void* GlobalAllocator::Alloc(i64 size, i32 const alignment)
{
  check((alignment == 1 || !(alignment & 0x1)) && alignment <= MEMORY_ALLOCATION_ALIGNMENT, "Alignment must be a power of 2 and up to 16-byte aligned.");
  return HeapAlloc(MemHandle, HEAP_ZERO_MEMORY, size);
}

void GlobalAllocator::Free(void const* p)
{
  HeapFree(MemHandle, 0, (void*)p);
}

bool GlobalAllocator::FollowsContainerDuringMove()
{
  return true;
}

bool GlobalAllocator::OwnedByContainer()
{
  return false;
}

GlobalAllocator::GlobalAllocator()
{
  if (MemHandle)
    return;

  MemHandle = GetProcessHeap();
  if (!MemHandle)
    MemHandle = HeapCreate(0, 0, 0);
}
} // namespace Core

#pragma warning(disable : 28'251)
void* operator new(std::size_t count)
{
  return globalAllocatorInstance.Alloc((int)count, MEMORY_ALLOCATION_ALIGNMENT);
}

void* operator new[](std::size_t count)
{
  return globalAllocatorInstance.Alloc((int)count, MEMORY_ALLOCATION_ALIGNMENT);
}

void* operator new(std::size_t count, std::align_val_t al)
{
  return globalAllocatorInstance.Alloc((int)count, (int)al);
}

void* operator new[](std::size_t count, std::align_val_t al)
{
  return globalAllocatorInstance.Alloc((int)count, (int)al);
}

void* operator new(std::size_t count, std::nothrow_t const&) noexcept
{
  return globalAllocatorInstance.Alloc((int)count, MEMORY_ALLOCATION_ALIGNMENT);
}

void* operator new[](std::size_t count, std::nothrow_t const&) noexcept
{
  return globalAllocatorInstance.Alloc((int)count, MEMORY_ALLOCATION_ALIGNMENT);
}

void* operator new(std::size_t count, std::align_val_t al, std::nothrow_t const&) noexcept
{
  return globalAllocatorInstance.Alloc((int)count, (int)al);
}

void* operator new[](std::size_t count, std::align_val_t al, std::nothrow_t const&) noexcept
{
  return globalAllocatorInstance.Alloc((int)count, (int)al);
}

void operator delete(void* ptr) noexcept
{
  globalAllocatorInstance.Free(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept
{
  globalAllocatorInstance.Free(ptr);
}

void operator delete[](void* ptr) noexcept
{
  globalAllocatorInstance.Free(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept
{
  globalAllocatorInstance.Free(ptr);
}
#pragma warning(default : 28'251)
