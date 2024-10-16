#pragma once

#include <Core/Allocator/Allocator.h>

namespace Core
{
class CORE_API GlobalAllocator final : public IAllocator
{
public:
  // Inherited via IAllocator
  __declspec(allocator) __declspec(restrict) void* Alloc(i64 const size, i32 const alignment) override;
  __declspec(allocator) __declspec(restrict) virtual void* Realloc(__declspec(noalias) void* p, i64 const size, i32 const alignment) override;
  void Free(__declspec(noalias) void* p) override;
  bool IsMovable() override;
  bool IsCopyable() override;
  bool OwnedByContainer() override;
};
} // namespace Core
