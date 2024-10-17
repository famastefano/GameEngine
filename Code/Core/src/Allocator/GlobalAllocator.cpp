#include <Core/Allocator/GlobalAllocator.h>
#include <Core/Assert/Assert.h>
#include <Windows.h>
#include <intrin.h>
#include <mutex>
#include <new>

static Core::GlobalAllocator globalAllocatorInstance;

namespace Core
{
IAllocator* globalAllocator = &globalAllocatorInstance;

static HANDLE     MemHandle = GetProcessHeap() ? GetProcessHeap() : HeapCreate(0, 0, 0);
static std::mutex GlobalAllocatorMutex;

struct AllocMetadata
{
  void* OverAligned;
  void* Original;
};
static i32            MetaDataSize     = 0;
static i32            MetaDataCapacity = 1'024;
static AllocMetadata* MetaData         = (AllocMetadata*)globalAllocator->Alloc(1'024 * sizeof(AllocMetadata), alignof(AllocMetadata));

constexpr bool operator==(AllocMetadata const& metadata, void* p)
{
  return metadata.OverAligned == p;
}

void           ReallocMetadata();
void           AddMetadata(AllocMetadata const& metadata);
void           RemoveMetadata(AllocMetadata* metadata);
AllocMetadata* FindMetadata(void* p);

constexpr bool IsAlignedTo(void* p, i32 const alignment)
{
  return !((u64)p & (alignment - 1));
}

void* GlobalAllocator::Alloc(i64 size, i32 const alignment)
{
  check((alignment == 1 || !(alignment & 0x1)), "Alignment must be a power of 2.");

  if (alignment <= MEMORY_ALLOCATION_ALIGNMENT)
    return HeapAlloc(MemHandle, HEAP_ZERO_MEMORY, size);

  size                  += alignment;
  void* const allocated  = HeapAlloc(MemHandle, HEAP_ZERO_MEMORY, size);
  if (!allocated)
    return allocated;

  if (IsAlignedTo(allocated, alignment)) // Sometimes the memory is already aligned over than 16-byte for pure coincidence
    return allocated;

  unsigned long index;
  _BitScanForward(&index, alignment);
  void* const overaligned = (void*)((u64)allocated << (index - 4)); // already aligned to 16-bytes, so we only shift by the delta
  check(IsAlignedTo(overaligned, alignment), "Invalid alignment!");

  std::scoped_lock lck{GlobalAllocatorMutex};
  AddMetadata({.OverAligned = overaligned, .Original = allocated});
  return overaligned;
}

void* GlobalAllocator::Realloc(void* toRealloc, i64 size, i32 const alignment)
{
  check((alignment == 1 || !(alignment & 0x1)), "Alignment must be a power of 2.");

  if (alignment > MEMORY_ALLOCATION_ALIGNMENT)
    size += alignment;

  void* reallocated = HeapReAlloc(MemHandle, HEAP_REALLOC_IN_PLACE_ONLY | HEAP_ZERO_MEMORY, toRealloc, size);
  if (!reallocated)
    reallocated = HeapReAlloc(MemHandle, HEAP_ZERO_MEMORY, toRealloc, size);

  if (!reallocated || alignment <= MEMORY_ALLOCATION_ALIGNMENT)
    return reallocated;

  if (IsAlignedTo(reallocated, alignment)) // Sometimes the memory is already aligned over than 16-byte for pure coincidence
    return reallocated;

  unsigned long index;
  _BitScanForward(&index, alignment);
  void* const overaligned = (void*)((u64)reallocated << (index - 4)); // already aligned to 16-bytes, so we only shift by the delta
  check(IsAlignedTo(overaligned, alignment), "Invalid alignment!");

  std::scoped_lock lck{GlobalAllocatorMutex};
  AddMetadata({.OverAligned = overaligned, .Original = reallocated});
  return overaligned;
}

void GlobalAllocator::Free(void* p)
{
  {
    std::scoped_lock lck{GlobalAllocatorMutex};
    if (AllocMetadata* metadata = FindMetadata(p))
    {
      p = metadata->Original;
      RemoveMetadata(metadata);
    }
  }
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

void ReallocMetadata()
{
  i32 const newCap = MetaDataCapacity * 2;
  if (void* reallocated = globalAllocator->Realloc(MetaData, newCap, alignof(AllocMetadata)))
  {
    MetaData         = (AllocMetadata*)reallocated;
    MetaDataCapacity = newCap;
    return;
  }
  if (void* newAlloc = globalAllocator->Alloc(newCap, alignof(AllocMetadata)))
  {
    // We rely on the fact that the memory has been initialized with HEAP_ZERO_MEMORY here!
    std::memcpy(newAlloc, MetaData, sizeof(AllocMetadata) * MetaDataSize);
    globalAllocator->Free(MetaData);
    MetaData         = (AllocMetadata*)newAlloc;
    MetaDataCapacity = newCap;
    return;
  }
  std::terminate();
}

void AddMetadata(AllocMetadata const& metadata)
{
  if (MetaDataSize == MetaDataCapacity)
    ReallocMetadata();
  MetaData[MetaDataSize++] = metadata;
}

void RemoveMetadata(AllocMetadata* metadata)
{
  std::swap(MetaData[--MetaDataSize], *metadata);
}

AllocMetadata* FindMetadata(void* p)
{
  auto it = std::find(MetaData, MetaData + MetaDataSize, p);
  return it != MetaData + MetaDataSize ? it : nullptr;
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
