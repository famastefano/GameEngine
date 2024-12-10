#pragma once

#include <Core/Container/Vector.h>
#include <Engine/API.h>

namespace Engine
{
class EngineSubSystem;

class ENGINE_API GameEngine
{
  Core::Vector<EngineSubSystem*> EngineSubSystems;

public:
  GameEngine();
  virtual ~GameEngine();
  GameEngine(GameEngine const&)            = delete;
  GameEngine& operator=(GameEngine const&) = delete;

  virtual void PreInitialize();
  virtual void PostInitialize();

  virtual void Tick(f32 DeltaTime);
};
} // namespace Engine
