#pragma once

#include <Shared/API.h>

namespace Shared
{
class IGameEngine;

class SHARED_API IEnvironment
{
protected:
  static void RegisterGlobalEnvironment(IEnvironment* Environment);

public:
  virtual ~IEnvironment() = default;

  virtual IGameEngine* GetGameEngine() = 0;
};
} // namespace Shared

SHARED_API extern Shared::IEnvironment* GlobalEnvironment;
