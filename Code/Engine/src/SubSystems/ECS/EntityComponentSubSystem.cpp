﻿#include <Engine/SubSystems/ECS/EntityComponentSubSystem.h>

GE_DEFINE_TYPE_METADATA(Engine::EntityComponentSubSystem, Engine::TypeMetaData::EngineSubSystem)

namespace Engine
{
void EntityComponentSubSystem::PreInitialize()
{
  EngineSubSystem::PreInitialize();
}
void EntityComponentSubSystem::PostInitialize()
{
  EngineSubSystem::PostInitialize();
}
void EntityComponentSubSystem::Tick(f32 DeltaTime)
{
}
bool EntityComponentSubSystem::HandleEvent(EventBase& Event)
{
  return EngineSubSystem::HandleEvent(Event);
}
EntityComponentSubSystem::~EntityComponentSubSystem()
{
}
} // namespace Engine
