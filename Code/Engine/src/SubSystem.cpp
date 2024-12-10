#include <Engine/Private/LogEngine.h>
#include <Engine/SubSystems/SubSystem.h>

namespace Engine
{
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
} // namespace Engine
