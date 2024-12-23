#pragma once

#include <Core/Allocator/Allocator.h>

namespace Core
{
class CORE_API GlobalAllocator final : public IAllocator
{
public:
  static GlobalAllocator& GetInstance();

  // clang-format off
  // Inherited via IAllocator
  __declspec(allocator) __declspec(restrict) void* Alloc(i64 const size, i32 const alignment) override;
  __declspec(allocator) __declspec(restrict) __declspec(noalias) void* Realloc(void* p, i64 const size, i32 const alignment) override;
  __declspec(noalias) void Free(void* p, i32 const alignment) override;
  bool IsMovable() override;
  bool IsCopyable() override;
  bool OwnedByContainer() override;
  //clang-format on
};
} // namespace Core
