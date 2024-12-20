#pragma once

#include <Core/Definitions.h>
#include <Engine/API.h>
#include <Engine/Reflection/Reflection.h>

namespace Engine
{
struct EventBase;
class GameEngine;

class ENGINE_API SubSystem
{
  GE_DECLARE_CLASS_TYPE_METADATA_BASE()

protected:
  static GameEngine& GetGameEngine();

public:
  SubSystem()                            = default;
  SubSystem(SubSystem const&)            = delete;
  SubSystem(SubSystem&&)                 = delete;
  SubSystem& operator=(SubSystem const&) = delete;
  SubSystem& operator=(SubSystem&&)      = delete;
  virtual ~SubSystem()                   = default;

  virtual void Tick(f32 DeltaTime) = 0;

  virtual void PreInitialize();
  virtual void PostInitialize();

  virtual bool HandleEvent(EventBase& Event);
};

template<typename T>
concept SubSystemType = std::derived_from<T, SubSystem>;
} // namespace Engine
