#pragma once

#include <Engine/SubSystems/EngineSubSystem.h>

namespace Engine
{
class EntityComponentSubSystem : public EngineSubSystem
{
  GE_DECLARE_CLASS_TYPE_METADATA()

public:
  void PreInitialize() override;
  void PostInitialize() override;
  void Tick(f32 DeltaTime) override;
  bool HandleEvent(EventBase& Event) override;
  ~EntityComponentSubSystem() override;
};
} // namespace Engine
