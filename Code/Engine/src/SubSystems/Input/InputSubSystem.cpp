#include <Engine/Events/Input/InputEvents.h>
#include <Engine/Private/LogInput.h>
#include <Engine/SubSystems/Input/InputSubSystem.h>
#include <Engine/SubSystems/SubSystemRegistration.h>
#include <Windows.h>
#include <hidusage.h>

namespace Engine
{
GE_REGISTER_SUBSYSTEM(SubSystemType::Engine, InputSubSystem)

void InputSubSystem::Tick(f32 DeltaTime)
{
}
void InputSubSystem::PreInitialize()
{
  EngineSubSystem::PreInitialize();

  RAWINPUTDEVICE rid[2] = {};
  rid[0].dwFlags        = 0;
  rid[0].usUsagePage    = HID_USAGE_PAGE_GENERIC;
  rid[0].usUsage        = HID_USAGE_GENERIC_MOUSE;
  rid[0].hwndTarget     = nullptr;
  rid[1].dwFlags        = 0;
  rid[1].usUsagePage    = HID_USAGE_PAGE_GENERIC;
  rid[1].usUsage        = HID_USAGE_GENERIC_KEYBOARD;
  rid[1].hwndTarget     = nullptr;
  bool const success    = RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE));
  checkf(success, "Failed to register raw input");
}
bool InputSubSystem::HandleEvent(EventBase& Event)
{
  if (Event.GetID() == EventInput::GetUniqueID())
  {
    using enum Kind;
    EventInput& ev = (EventInput&)Event;
    switch (ev.Kind_)
    {
    case Mouse_Press:
    case Mouse_Release:
    case Mouse_Move:
    case Mouse_Wheel:
      GE_LOG(LogInput, Core::Verbosity::Error, "Mouse Input");
      break;
    case Key_Press:
    case Key_Release:
      GE_LOG(LogInput, Core::Verbosity::Error, "Keyboard Input");
      break;
    }
    return true;
  }
  return false;
}
} // namespace Engine
