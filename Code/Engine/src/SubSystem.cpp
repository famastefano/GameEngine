#include <Engine/LogEngine.h>
#include <Engine/SubSystems/SubSystem.h>
#include <Engine/Interfaces/IEnvironment.h>
#include <Engine/GameEngine/GameEngine.h>

namespace Engine
{
GameEngine& SubSystem::GetGameEngine()
{
  check(GlobalEnvironment);
  return GlobalEnvironment->GetGameEngine();
}
void SubSystem::PreInitialize()
{
  auto const name = Name();
  GE_LOG(LogEngine, Core::Verbosity::Info, "SubSystem `%*.s`", name.Size(), name.Data());
}
void SubSystem::PostInitialize()
{
  auto const name = Name();
  GE_LOG(LogEngine, Core::Verbosity::Info, "SubSystem `%*.s`", name.Size(), name.Data());
}
bool SubSystem::HandleEvent(EventBase& Event)
{
  return false;
}
} // namespace Engine
