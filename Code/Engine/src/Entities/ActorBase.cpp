#include <Engine/Entities/ActorBase.h>

namespace Engine::Entities
{
Actor::Actor(char const*& Name) : Name_(Name)
{
}
Core::StringView<char> Actor::Name() const
{
  return Name_.AsView();
}
void Actor::PreInitialize()
{
}
void Actor::PostInitialize()
{
}
void Actor::PreDeinitialize()
{
}
void Actor::PostDeinitialize()
{
}
void Actor::PreTickComponents(f32 DeltaTime)
{
}
void Actor::TickComponents(f32 DeltaTime)
{
  for (auto* component : Components_)
    component->Tick(DeltaTime);
}
void Actor::PostTickComponents(f32 DeltaTime)
{
}
} // namespace Engine::Entities
