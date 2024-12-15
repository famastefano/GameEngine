#include <Engine/Interfaces/IEnvironment.h>

namespace Engine
{
void IEnvironment::RegisterGlobalEnvironment(IEnvironment* Environment)
{
  GlobalEnvironment = Environment;
}
IEnvironment::IEnvironment()
{
  RegisterGlobalEnvironment(this);
}
} // namespace Engine

Engine::IEnvironment* GlobalEnvironment{};
