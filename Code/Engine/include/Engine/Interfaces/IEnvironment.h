#pragma once

#include <Engine/API.h>

namespace Engine
{
class GameEngine;

class ENGINE_API IEnvironment
{
protected:
  static void RegisterGlobalEnvironment(IEnvironment* Environment);

public:
  using NativeLoopCallbackFn = void(*)();
  NativeLoopCallbackFn NativeLoopCallback_{};

  IEnvironment();
  virtual ~IEnvironment() = default;

  virtual GameEngine& GetGameEngine() = 0;

  enum class RunningMode
  {
    Client,
    Server,
    Standalone,
    Tool,
  };
  virtual RunningMode GetRunningMode() = 0;
};
} // namespace Engine

ENGINE_API extern Engine::IEnvironment* GlobalEnvironment;
