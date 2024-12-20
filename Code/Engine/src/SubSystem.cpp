#include <Engine/GameEngine/GameEngine.h>
#include <Engine/Interfaces/IEnvironment.h>
#include <Engine/LogEngine.h>
#include <Engine/SubSystems/SubSystem.h>

namespace Engine
{
GameEngine& SubSystem::GetGameEngine()
{
  check(GlobalEnvironment);
  return GlobalEnvironment->GetGameEngine();
}
void SubSystem::PreInitialize()
{
  GE_LOG(LogEngine, Core::Verbosity::Info, "SubSystem `%s`", GetTypeMetaData().Name_);
}
void SubSystem::PostInitialize()
{
  GE_LOG(LogEngine, Core::Verbosity::Info, "SubSystem `%s`", GetTypeMetaData().Name_);
}
bool SubSystem::HandleEvent(EventBase&)
{
  return false;
}
} // namespace Engine
