#pragma once

#include <Engine/SubSystems/EngineSubSystem.h>

namespace Engine
{
class InputSubSystem : public EngineSubSystem
{
public:
  void Tick(f32 DeltaTime) override;
  void PreInitialize() override;
};
}