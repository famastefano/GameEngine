#include <Core/Assert/Assert.h>
#include <Engine/Components/ComponentBase.h>
#include <Engine/Events/ECS/ComponentAttached.h>
#include <Engine/Events/ECS/ComponentDetached.h>
#include <Engine/GameEngine/GameEngine.h>
#include <Engine/Interfaces/IEnvironment.h>

namespace Engine::Components
{
Entities::ActorBase* ComponentBase::Owner() const
{
  return Owner_;
}
void ComponentBase::PreAttach(Entities::ActorBase& NewOwner)
{
  check(!Owner_);
  Owner_ = &NewOwner;
}
void ComponentBase::PostAttach(Entities::ActorBase& NewOwner)
{
  check(Owner_ == &NewOwner);
  EventComponentAttached ev;
  ev.NewOwner_  = &NewOwner;
  ev.Component_ = this;
  GlobalEnvironment->GetGameEngine().EnqueueEvent(ev);
}
void ComponentBase::PreDetach(Entities::ActorBase& PrevOwner)
{
  (void)PrevOwner;
  check(Owner_ == &PrevOwner);
}
void ComponentBase::PostDetach(Entities::ActorBase& PrevOwner)
{
  (void)PrevOwner;
  check(Owner_ == &PrevOwner);
  Owner_ = nullptr;

  EventComponentDetached ev;
  ev.PrevOwner_ = &PrevOwner;
  ev.Component_ = this;
  GlobalEnvironment->GetGameEngine().EnqueueEvent(ev);
}
void ComponentBase::Tick(f32)
{
}
} // namespace Engine::Components
