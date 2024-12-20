#pragma once

#include <Engine/SubSystems/EngineSubSystem.h>

namespace Engine
{
class InputSubSystem : public EngineSubSystem
{
  GE_DECLARE_CLASS_TYPE_METADATA()

public:
  void Tick(f32 DeltaTime) override;
  void PreInitialize() override;
  bool HandleEvent(EventBase& Event) override;
};
}