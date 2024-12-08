#include <Engine/Interfaces/IEnvironment.h>

namespace Engine
{
Engine::IEnvironment* GlobalEnvironment{};

void IEnvironment::RegisterGlobalEnvironment(IEnvironment* Environment)
{
  GlobalEnvironment = Environment;
}
} // namespace Engine
