#pragma once

#include <Engine/API.h>

namespace Engine
{
class IGameEngine;

class ENGINE_API IEnvironment
{
protected:
  static void RegisterGlobalEnvironment(IEnvironment* Environment);

public:
  virtual ~IEnvironment() = default;

  virtual IGameEngine* GetGameEngine() = 0;
};
} // namespace Engine

ENGINE_API extern Engine::IEnvironment* GlobalEnvironment;
