#pragma once

#include <Engine/Events/EventBase.h>

namespace Engine
{
struct ENGINE_API EventResizeWindow : public EventBase
{
  GE_DECLARE_STRUCT_TYPE_METADATA()

  EventResizeWindow(i32 const Width, i32 const Height)
      : Width_(Width)
      , Height_(Height)
  {
  }

  i32 Width_, Height_;
};
} // namespace Engine
