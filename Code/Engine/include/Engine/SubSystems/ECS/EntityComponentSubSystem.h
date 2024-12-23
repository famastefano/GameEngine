#pragma once

#include <Core/Container/FlatMap.h>
#include <Engine/Entities/ActorBase.h>
#include <Engine/SubSystems/EngineSubSystem.h>

namespace Engine
{
class EntityComponentSubSystem : public EngineSubSystem
{
  GE_DECLARE_CLASS_TYPE_METADATA()

  Core::CompactFlatMap<u64, Entities::ActorBase*> Actors_;

public:
  void PreInitialize() override;
  void PostInitialize() override;
  void Tick(f32 DeltaTime) override;
  bool HandleEvent(EventBase& Event) override;
  ~EntityComponentSubSystem() override;

  Entities::ActorBase* SpawnActor(u64 ClassID, Core::StringView<char> Name, Math::Vec3Df WorldPosition = {});
  void DestroyActor(Entities::ActorBase* Actor);
  void DestroyActor(u64 ID);

  template <Entities::ActorType T>
  T* SpawnActor(Core::StringView<char> const Name, Math::Vec3Df const WorldPosition = {})
  {
    return (T*)SpawnActor(T::GetStaticTypeMetaData().ID_, Name, WorldPosition);
  }
};
} // namespace Engine
