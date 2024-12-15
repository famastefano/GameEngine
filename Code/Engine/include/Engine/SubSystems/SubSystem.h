#pragma once

#include <Core/Container/String.h>
#include <Core/Definitions.h>
#include <Engine/API.h>

namespace Engine
{
struct EventBase;
class GameEngine;

class ENGINE_API SubSystem
{
protected:
  static GameEngine& GetGameEngine();

public:
  SubSystem() = default;
  SubSystem(SubSystem const&)            = delete;
  SubSystem(SubSystem&&)                 = delete;
  SubSystem& operator=(SubSystem const&) = delete;
  SubSystem& operator=(SubSystem&&)      = delete;
  virtual ~SubSystem()                   = default;

  virtual void Tick(f32 DeltaTime) = 0;

  virtual Core::StringView<char> Name() const = 0;

  virtual void PreInitialize();
  virtual void PostInitialize();

  virtual bool HandleEvent(EventBase& Event);
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
