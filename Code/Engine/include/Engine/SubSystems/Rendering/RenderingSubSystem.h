#pragma once

#include <Core/Container/FlatMap.h>
#include <Engine/SubSystems/EngineSubSystem.h>

namespace Engine
{
struct EventBase;

namespace Components
{
class ComponentBase;
}

class ENGINE_API RenderingSubSystem : public EngineSubSystem
{
  GE_DECLARE_CLASS_TYPE_METADATA()

  Core::FlatMap<u64, Core::Vector<Components::ComponentBase*>> RenderingComponents_;
  Core::CompactFlatMap<u64, u32>                               Shaders_;

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
  void        Cleanup();
  static bool IsComponentHandledByUs(u64 ID);
  void        CompileShaders();
};
} // namespace Engine
