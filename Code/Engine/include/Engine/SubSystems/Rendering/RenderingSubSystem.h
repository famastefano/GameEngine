#pragma once

#include <Engine/SubSystems/EngineSubSystem.h>

namespace Engine
{
struct EventBase;

class RenderingSubSystem : public EngineSubSystem
{
  GE_IMPLEMENT_SUBSYSTEM(RenderingSubSystem)

public:
  using Super = EngineSubSystem;

  // Size of the tile
  i32 const TileSize_ = 16;

  // EngineSubSystem
  RenderingSubSystem();
  void Tick(f32 DeltaTime) override;
  ~RenderingSubSystem() override;

  void PreInitialize() override;
  void PostInitialize() override;

  bool HandleEvent(EventBase& Event) override;
  // ---

private:
  static void ResizeViewport(i32 Width, i32 Height);
  static void Cleanup();
  static bool IsComponentHandledByUs(u64 ID);
};
} // namespace Engine
