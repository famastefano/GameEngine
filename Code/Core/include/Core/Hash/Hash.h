#pragma once

#include <Core/API.h>
#include <Core/Definitions.h>

namespace Core
{
CORE_API u64 CalculateHash(void const* p, i32 const bytes);

template <typename R, typename... Args>
u64 CalculateHash(R (*Func)(Args...))
{
  return Func;
}

template <typename T, typename R, typename... Args>
u64 CalculateHash(T* Instance, R (T::*Method)(Args...))
{
#pragma pack(1)
  struct Dummy
  {
    T*               Instance_ = Instance;
    decltype(Method) Method_   = Method;
  } Dummy_;
  return CalculateHash(&Dummy_, sizeof(Dummy_));
}
} // namespace Core
