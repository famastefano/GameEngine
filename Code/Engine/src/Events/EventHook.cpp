#include <Engine/Events/EventHook.h>

namespace Engine
{
void EventHook::Next(EventBase& Event) const
{
  if (Next_)
    Next_->HandleEvent(Event);
}

void EventHook::SendEvent(EventBase& Event)
{
  if (Head_)
    Head_->HandleEvent(Event);
}
} // namespace Engine
