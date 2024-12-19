#pragma once

#include <Core/Definitions.h>
#include <Engine/Reflection/Reflection.h>
#include <Engine/API.h>

namespace Engine::Entities
{
class ActorBase;
}

namespace Engine::Components
{
class ENGINE_API ComponentBase
{
  GE_DECLARE_CLASS_TYPE_METADATA_BASE()

  Entities::ActorBase* Owner_{};

public:
  ComponentBase()                                = default;
  ComponentBase(ComponentBase const&)            = delete;
  ComponentBase& operator=(ComponentBase const&) = delete;
  ComponentBase(ComponentBase&&)                 = delete;
  ComponentBase& operator=(ComponentBase&&)      = delete;
  virtual ~ComponentBase()                       = default;

  Entities::ActorBase* Owner() const;

  virtual void PreAttach(Entities::ActorBase& NewOwner);
  virtual void PostAttach(Entities::ActorBase& NewOwner);
  virtual void PreDetach(Entities::ActorBase& PrevOwner);
  virtual void PostDetach(Entities::ActorBase& PrevOwner);
  virtual void Tick(f32 DeltaTime);
};
} // namespace Engine::Components
