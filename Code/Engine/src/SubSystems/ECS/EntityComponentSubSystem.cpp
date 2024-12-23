#include <Engine/SubSystems/ECS/EntityComponentSubSystem.h>

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
  for (auto* actor : Actors_.Values())
  {
    actor->Deinitialize();
    delete actor;
  }
}
Entities::ActorBase* EntityComponentSubSystem::SpawnActor(u64 ClassID, Core::StringView<char> Name, Math::Vec3Df WorldPosition)
{
  if (auto const** p = GetTypesMetaData().Find(ClassID))
  {
    auto const& metaData = **p;
    if (metaData.Kind_ != TypeMetaData::Actor)
      return nullptr;

    Entities::ActorBase* actor = (Entities::ActorBase*)metaData.Factory_();

    actor->SetName(Name);
    actor->Transform_.Position_ = WorldPosition;
    return actor;
  }
  return nullptr;
}
void EntityComponentSubSystem::DestroyActor(Entities::ActorBase* Actor)
{
  Actor->Deinitialize();
  Actors_.TryRemove(Actor->ID());
}
void EntityComponentSubSystem::DestroyActor(u64 const ID)
{
  if (auto** actor = Actors_.Find(ID))
    DestroyActor(*actor);
}
} // namespace Engine
