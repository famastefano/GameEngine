#pragma once

#include <Engine/Events/EventBase.h>

namespace Engine
{
namespace Entities
{
class ActorBase;
}
namespace Components
{
class ComponentBase;
}
struct EventComponentDetached : EventBase
{
  GE_DECLARE_STRUCT_TYPE_METADATA()

  Entities::ActorBase* PrevOwner_{};
  Components::ComponentBase* Component_{};
};
}; // namespace Engine
