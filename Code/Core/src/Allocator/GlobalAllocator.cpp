#include <Core/Allocator/GlobalAllocator.h>
#include <Core/Assert/Assert.h>
#include <Windows.h>
#include <intrin.h>
#include <mutex>
#include <new>
#include <unordered_map>

static Core::GlobalAllocator globalAllocatorInstance;

namespace Core
{
IAllocator* globalAllocator = &globalAllocatorInstance;

static HANDLE                           MemHandle = GetProcessHeap() ? GetProcessHeap() : HeapCreate(0, 0, 0);
static std::mutex                       GlobalAllocatorMutex;

// TODO: refactor to use a flat map as we expect very specific optimized use-cases for data aligned over 16B
static std::unordered_map<void*, void*> OverAlignedPointers;

void* GlobalAllocator::Alloc(i64 size, i32 const alignment)
{
  check((alignment == 1 || !(alignment & 0x1)), "Alignment must be a power of 2.");

  if (alignment <= MEMORY_ALLOCATION_ALIGNMENT)
    return HeapAlloc(MemHandle, HEAP_ZERO_MEMORY, size);

  size             += alignment;
  void* const p     = HeapAlloc(MemHandle, HEAP_ZERO_MEMORY, size);
  u64         addr  = (u64)p;
  if (addr & (alignment - 1)) // Sometimes the memory is already aligned over than 16-byte for pure coincidence
    return p;

  unsigned long index;
  _BitScanForward(&index, alignment);
  addr <<= index - 4; // already aligned to 16-bytes, so we only shift by the delta
  check(addr & (alignment - 1), "Invalid alignment!");

  std::scoped_lock lck{GlobalAllocatorMutex};
  OverAlignedPointers[(void*)addr] = p;
  return (void*)addr;
}

void GlobalAllocator::Free(void* p)
{
  if (auto it = OverAlignedPointers.find(p); it != OverAlignedPointers.end())
    HeapFree(MemHandle, 0, it->second);
  HeapFree(MemHandle, 0, p);
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
