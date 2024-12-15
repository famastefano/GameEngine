#include <Engine/Events/Input/InputEvents.h>
#include <Engine/SubSystems/Input/InputSubSystem.h>
#include <Windows.h>
#include <hidusage.h>

namespace Engine
{
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
  const bool success = RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE));
  checkf(success, "Failed to register raw input");
}
bool InputSubSystem::HandleEvent(EventBase& Event)
{
  if (Event.GetID() == InputEvent::GetUniqueID())
  {
    return true;
  }
  return false;
}
} // namespace Engine
