#pragma once

#include <Core/Container/String.h>
#include <Core/Definitions.h>
#include <Engine/API.h>

namespace Engine
{
class GameEngine;

class ENGINE_API SubSystem
{
public:
  virtual ~SubSystem() = default;

  virtual void Tick(f32 DeltaTime) = 0;

  virtual Core::StringView<char> Name() const = 0;

  virtual void PreInitialize();
  virtual void PostInitialize();
};
} // namespace Engine

#define GE_IMPLEMENT_SUBSYSTEM(SubSystem)             \
public:                                               \
  inline Core::StringView<char> Name() const override \
  {                                                   \
    return #SubSystem;                                \
  }                                                   \
                                                      \
private:
