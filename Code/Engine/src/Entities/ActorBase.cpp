#include <Engine/Entities/ActorBase.h>

namespace Engine::Entities
{
void ActorBase::SetName(Core::String<char> Name)
{
  Name_ = std::move(Name);
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
} // namespace Engine::Entities
