﻿#include <Core/Assert/Assert.h>
#include <Engine/Components/ComponentBase.h>

namespace Engine::Components
{
Entities::ActorBase* ComponentBase::Owner() const
{
  return Owner_;
}
void ComponentBase::PreAttach(Entities::ActorBase& NewOwner)
{
  check(!Owner_);
}
void ComponentBase::PostAttach(Entities::ActorBase& NewOwner)
{
  check(!Owner_);
}
void ComponentBase::PreDetach(Entities::ActorBase& PrevOwner)
{
  check(Owner_ == &PrevOwner);
}
void ComponentBase::PostDetach(Entities::ActorBase& PrevOwner)
{
  check(Owner_ == &PrevOwner);
  Owner_ = nullptr;
}
void ComponentBase::Tick(f32 DeltaTime)
{
}
} // namespace Engine::Components