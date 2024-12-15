#pragma once

#include <Engine/SubSystems/EngineSubSystem.h>

namespace Engine
{
struct EventBase;

class RendererSubSystem : public EngineSubSystem
{
  GE_IMPLEMENT_SUBSYSTEM(RendererSubSystem)

public:
  using Super = EngineSubSystem;

  // Size of the tile
  i32 const TileSize_ = 16;

  RendererSubSystem();
  void Tick(f32 DeltaTime) override;
  ~RendererSubSystem() override;

  void PreInitialize() override;

  bool HandleEvent(EventBase& Event) override;

private:
  void ResizeViewport(i32 const Width, i32 const Height);
};
} // namespace Engine
