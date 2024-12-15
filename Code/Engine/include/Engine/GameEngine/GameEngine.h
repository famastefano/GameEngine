#pragma once

#include <Core/Container/FlatMap.h>
#include <Core/Container/String.h>
#include <Engine/API.h>
#include <Engine/Events/EventBase.h>
#include <Engine/Events/Renderer/EventResizeWindow.h>

namespace Engine
{
class EngineSubSystem;

class ENGINE_API GameEngine
{
  Core::CompactFlatMap<u64, EngineSubSystem*> EngineSubSystems_;

public:
  GameEngine();
  virtual ~GameEngine();
  GameEngine(GameEngine const&)            = delete;
  GameEngine& operator=(GameEngine const&) = delete;

  virtual void PreInitialize();
  virtual void PostInitialize();

  virtual void Tick(f32 DeltaTime);

  virtual EngineSubSystem* GetEngineSubSystem(Core::StringView<char> Name);

  virtual void EnqueueEvent(EventBase& Event);
};
} // namespace Engine
