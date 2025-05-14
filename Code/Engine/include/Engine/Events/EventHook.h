#pragma once

#include <Engine/API.h>
#include <Engine/Events/EventBase.h>

namespace Engine
{
class ENGINE_API EventHook
{
  inline static EventHook* Head_{};
  EventHook* Next_{};

protected:
  void Next(EventBase& Event) const;

public:
  virtual ~EventHook() = default;

  // Handles the event.
  // You have to manually call `Next(Event)` to propagate the event along the chain.
  virtual void HandleEvent(EventBase& Event) const = 0;

  template<typename THookClass, typename... TArgs>
  static void RegisterHook(TArgs&&... Args)
  {
    auto* NewHook = new THookClass(std::forward<TArgs>(Args)...);
    NewHook->Next_ = Head_;
    Head_ = NewHook;
  }

  static void SendEvent(EventBase& Event);
};
}
