#include <Core/Assert/Assert.h>
#include <Engine/SubSystems/Input/Base/Translator.h>
#include <Windows.h>
#include <bit>

namespace Engine
{
namespace
{
bool TranslateWin32_WM_INPUT(NativeEvent const Event, EventInput& Translated)
{
  if (GET_RAWINPUT_CODE_WPARAM(Event.Windows_.WParam_) != RIM_INPUT)
    return false;

  HRAWINPUT hraw = (HRAWINPUT)Event.Windows_.LParam_;
  RAWINPUT  rawData{};
  UINT      pcbSize = sizeof(RAWINPUT);
  if (GetRawInputData(hraw, RID_INPUT, &rawData, &pcbSize, sizeof(RAWINPUTHEADER)) <= 0 || rawData.header.dwType == RIM_TYPEHID)
    return false;

  if (rawData.header.dwType == RIM_TYPEMOUSE)
  {
    struct ButtonInfo
    {
      Kind               Kind_;
      MouseEvent::Button Button_;
    };
    constexpr ButtonInfo infos[] = {
        {Kind::Mouse_Press, MouseEvent::Mouse0},
        {Kind::Mouse_Release, MouseEvent::Mouse0},
        {Kind::Mouse_Press, MouseEvent::Mouse1},
        {Kind::Mouse_Release, MouseEvent::Mouse1},
        {Kind::Mouse_Press, MouseEvent::Mouse2},
        {Kind::Mouse_Release, MouseEvent::Mouse2},
        {Kind::Mouse_Press, MouseEvent::Mouse3},
        {Kind::Mouse_Release, MouseEvent::Mouse3},
        {Kind::Mouse_Press, MouseEvent::Mouse4},
        {Kind::Mouse_Release, MouseEvent::Mouse4},
        {Kind::Mouse_Wheel, MouseEvent::NoButton},
        {Kind::Mouse_Wheel, MouseEvent::NoButton},
        {Kind::Mouse_Move, MouseEvent::NoButton},
        {Kind::Mouse_Move, MouseEvent::NoButton},
        {Kind::Mouse_Move, MouseEvent::NoButton},
        {Kind::Mouse_Move, MouseEvent::NoButton},
        {Kind::Mouse_Move, MouseEvent::NoButton},
    };
    const RAWMOUSE& mouse = rawData.data.mouse;
    int const       index = std::countr_zero(mouse.usButtonFlags);
    Translated.Kind_      = infos[index].Kind_;

    MouseEvent ev{};
    ev.Button_ = infos[index].Button_;
    ev.WheelY  = mouse.usButtonFlags & RI_MOUSE_WHEEL ? mouse.usButtonData : 0;
    if (mouse.usFlags == MOUSE_MOVE_RELATIVE)
    {
      ev.MoveX_ = (i16)mouse.lLastX;
      ev.MoveY_ = (i16)mouse.lLastY;
    }
    Translated.Mouse_ = ev;
  }
  else
  {
    const RAWKEYBOARD& kbd = rawData.data.keyboard;
    if (kbd.MakeCode == KEYBOARD_OVERRUN_MAKE_CODE || kbd.VKey >= UCHAR_MAX)
      return false;

    KeyboardEvent ev{};
    ev.NativeVirtualKey_ = kbd.VKey;

    HKL kbdLayout = GetKeyboardLayout(0);

    if (kbd.MakeCode)
    {
      // Compose the full scan code value with its extended byte
      ev.ScanCode_ = MAKEWORD(kbd.MakeCode & 0x7F, ((kbd.Flags & RI_KEY_E0) ? 0xE0 : ((kbd.Flags & RI_KEY_E1) ? 0xE1 : 0x00)));
    }
    else
    {
      // Scan code value may be empty for some buttons (for example multimedia buttons)
      // Try to get the scan code from the virtual key code
      ev.ScanCode_ = LOWORD(MapVirtualKeyEx(kbd.VKey, MAPVK_VK_TO_VSC_EX, kbdLayout));
    }

    BYTE kbdState[256];
    if (GetKeyboardState(kbdState) && ToUnicodeEx(kbd.VKey, kbd.MakeCode, kbdState, &ev.Char_, 1, 0, kbdLayout) <= 0)
      ev.Char_ = 0;

    constexpr Kind kinds[] = {Kind::Key_Press, Kind::Key_Release};
    Translated.Kind_       = kinds[kbd.Flags & 0x1];
    Translated.Keyboard_   = ev;
  }
  return true;
}
} // namespace

bool TranslateNativeEvent(NativeEvent const Event, EventInput& Translated)
{
  switch (Event.Windows_.Message_)
  {
  case WM_INPUT:
    return TranslateWin32_WM_INPUT(Event, Translated);
  }
  return false;
}
} // namespace Engine
