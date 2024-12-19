#pragma once

#include <Engine/Reflection/Reflection.h>
#include <Engine/API.h>

namespace Engine
{
struct ENGINE_API EventBase
{
  GE_DECLARE_STRUCT_TYPE_METADATA_BASE()

  EventBase() = default;
  virtual ~EventBase() = default;
};
} // namespace Engine
