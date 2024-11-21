#pragma once

#include <Core/Container/FlatMap.h>
#include <Core/Hash/Hash.h>
#include <functional>

namespace Core
{
template <typename... Args>
class Delegate
{
  Core::CompactFlatMap<u64, std::function<void(Args...)>> Listeners_;

public:
  constexpr Delegate()                           = default;
  constexpr Delegate(Delegate const&)            = delete;
  constexpr Delegate& operator=(Delegate const&) = delete;

  constexpr Delegate(Delegate&&) = default;
  constexpr Delegate& operator=(Delegate&& Other)
  {
    Listeners_ = std::move(Other.Listeners_);
    return *this;
  }

  template <typename R>
  constexpr void AddListener(R (*Func)(Args...))
  {
    Listeners_.TryEmplace(CalculateHash(Func), Func);
  }

  template <typename T, typename R>
  constexpr void AddListener(T* Instance, R (T::*Method)(Args...))
  {
    Listeners_.TryEmplace(CalculateHash(Instance, Method), [Instance, Method](Args&&... args) { (Instance->*Method)(std::forward<Args>(args)...); });
  }

  template <typename R>
  void RemoveListener(R (*Func)(Args...))
  {
    Listeners_.TryErase(CalculateHash(Func));
  }

  template <typename T, typename R>
  void RemoveListener(T* Instance, R (T::*Method)(Args...))
  {
    Listeners_.TryErase(CalculateHash(Instance, Method));
  }

  template <typename... UArgs>
  void Broadcast(UArgs&&... uargs)
  {
    for (auto& listener : Listeners_.Values())
      listener(std::forward<UArgs>(uargs)...);
  }
};
} // namespace Core
