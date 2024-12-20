#pragma once

#include <Core/Container/FlatMap.h>
#include <Core/Container/String.h>
#include <Engine/API.h>
#include <Engine/Components/ComponentBase.h>
#include <Engine/Reflection/Reflection.h>

namespace Engine::Entities
{
class ENGINE_API ActorBase
{
  GE_DECLARE_CLASS_TYPE_METADATA_BASE()

  Core::CompactFlatMap<u64, Components::ComponentBase*> Components_;

  Core::String<char> Name_;

public:
  ActorBase()                            = default;
  ActorBase(ActorBase const&)            = delete;
  ActorBase& operator=(ActorBase const&) = delete;
  ActorBase(ActorBase&& actor)           = delete;
  ActorBase& operator=(ActorBase&&)      = delete;
  virtual ~ActorBase()                   = default;

  void SetName(Core::String<char> Name);

  [[nodiscard]] Core::StringView<char> Name() const;

  virtual void PreInitialize();
  virtual void PostInitialize();
  virtual void PreDeinitialize();
  virtual void PostDeinitialize();
  virtual void PreTickComponents(f32 DeltaTime);
  void         TickComponents(f32 DeltaTime);
  virtual void PostTickComponents(f32 DeltaTime);

  template <Components::Component Component>
  [[nodiscard]] Component* AttachComponent()
  {
    u64 const ID = Component::GetStaticTypeMetaData().ID_;
    if (Components_.Contains(ID))
      return nullptr;

    auto* component = new Component();
    component->PreAttach(*this);

    bool const success = Components_.TryEmplace(ID, component);
    check(success);

    component->PostAttach(*this);
    return component;
  }

  template <Components::Component Component>
  void DetachComponent()
  {
    u64 const ID        = Component::GetStaticTypeMetaData().ID_;
    auto*     component = FindComponent<Component>();
    if (verifyf(component != nullptr, "Component %s not found in %.*s.", Component::GetStaticTypeMetaData().Name_, Name_.Size(), Name_.Data()))
    {
      component->PreDetach(*this);
      bool const success = Components_.TryRemove(ID);
      check(success);
      component->PostDetach(*this);
    }
  }

  template <Components::Component Component>
  [[nodiscard]] Component* FindComponent()
  {
    u64 const ID    = Component::GetStaticTypeMetaData().ID_;
    auto**    found = Components_.Find(ID);
    return found ? (Component*)*found : nullptr;
  }

  template <Components::Component Component>
  [[nodiscard]] Component const* FindComponent() const
  {
    u64 const    ID    = Component::GetStaticTypeMetaData().ID_;
    auto* const* found = Components_.Find(ID);
    return found ? (Component const*)*found : nullptr;
  }

  template <Components::Component Component>
  [[nodiscard]] Component* FindComponentChecked()
  {
    auto* found = FindComponent<Component>();
    checkf(found, "Failed to find component %s for actor %.*s.", Component::GetStaticTypeMetaData().Name_, Name().Size(), Name().Data());
    return found;
  }

  template <Components::Component Component>
  [[nodiscard]] Component const* FindComponentChecked() const
  {
    auto const* found = FindComponent<Component>();
    checkf(found, "Failed to find component %s for actor %.*s.", Component::GetStaticTypeMetaData().Name_, Name().Size(), Name().Data());
    return found;
  }
};
} // namespace Engine::Entities
