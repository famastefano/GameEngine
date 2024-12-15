#pragma once

#include <Engine/Events/EventBase.h>

namespace Engine
{
struct EventResizeWindow : public EventBase
{
  GE_IMPLEMENT_EVENT(EventResizeWindow)

  EventResizeWindow(i32 const Width, i32 const Height)
      : Width_(Width)
      , Height_(Height)
  {
  }

  i32 Width_, Height_;
};
} // namespace Engine
