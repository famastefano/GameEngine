#include <Shared/Interfaces/IEnvironment.h>

namespace Shared
{
Shared::IEnvironment* GlobalEnvironment{};

void IEnvironment::RegisterGlobalEnvironment(IEnvironment* Environment)
{
  GlobalEnvironment = Environment;
}
} // namespace Shared
