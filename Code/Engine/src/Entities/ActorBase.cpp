#include <Engine/Entities/ActorBase.h>

namespace Engine::Entities
{
namespace
{
std::atomic<i32> ActorIDCounter{0};
}

ActorBase::ActorBase()
    : ID_(ActorIDCounter.fetch_add(1, std::memory_order_relaxed))
{
}
void ActorBase::SetName(Core::String<char> Name)
{
  Name_ = std::move(Name);
}
u64 ActorBase::ID() const
{
  return ID_;
}
Core::StringView<char> ActorBase::Name() const
{
  return Name_.AsView();
}
void ActorBase::PreInitialize()
{
}
void ActorBase::PostInitialize()
{
}
void ActorBase::PreDeinitialize()
{
}
void ActorBase::PostDeinitialize()
{
}
void ActorBase::PreTickComponents(f32 DeltaTime)
{
}
void ActorBase::TickComponents(f32 DeltaTime)
{
  for (auto&& [K, V] : Components_)
    V->Tick(DeltaTime);
}
void ActorBase::PostTickComponents(f32 DeltaTime)
{
}
void ActorBase::Deinitialize()
{
  PreDeinitialize();
  for (auto* component : Components_.Values())
    component->PreDetach(*this);
  PostDeinitialize();
  for (auto* component : Components_.Values())
    component->PostDetach(*this);
  Components_.Clear();
}
} // namespace Engine::Entities
