#pragma once

#include <Core/Container/Vector.h>
#include <Engine/API.h>
#include <Engine/Events/EventBase.h>
#include <Engine/Reflection/Reflection.h>
#include <Engine/SubSystems/EngineSubSystem.h>

namespace Engine
{
class EngineSubSystem;

class ENGINE_API GameEngine
{
  GE_DECLARE_CLASS_TYPE_METADATA();

  Core::Vector<EngineSubSystem*> EngineSubSystems_;

public:
  GameEngine();
  virtual ~GameEngine();
  GameEngine(GameEngine const&)            = delete;
  GameEngine& operator=(GameEngine const&) = delete;

  virtual void PreInitialize();
  virtual void PostInitialize();

  virtual void Tick(f32 DeltaTime);

  virtual void EnqueueEvent(EventBase& Event);

  template <SubSystemType T>
  T* FindSubSystem()
  {
    auto const* SubSystem = EngineSubSystems_.Find([](EngineSubSystem const* SubSystem) { return SubSystem->GetTypeMetaData().ID_ == T::GetStaticTypeMetaData().ID_; });
    return (T*)SubSystem;
  }
};
} // namespace Engine
