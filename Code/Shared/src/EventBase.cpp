#include <Shared/Events/EventBase.h>
#include <atomic>

namespace Shared
{
static std::atomic<int> GlobalEventIDCounter = 0;

i32 EventBase::GenerateUniqueEventID()
{
  return GlobalEventIDCounter.fetch_add(1, std::memory_order_acq_rel);
}
} // namespace Shared
