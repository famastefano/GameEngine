#pragma once

#include <Engine/Events/EventBase.h>

namespace Engine
{
struct ENGINE_API EventResizeWindow : public EventBase
{
  GE_DECLARE_STRUCT_TYPE_METADATA()

  i32 Width_ = 0, Height_ = 0;
};
} // namespace Engine
