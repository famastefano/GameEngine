#include <Core/Assert/Assert.h>
#include <Core/Helpers.h>
#include <Input/Base/Translator.h>
#include <Windows.h>
#include <bit>

namespace Input::Private
{
bool TranslateWin32_WM_INPUT(NativeEvent const event, InputEvent& translated);
}

bool Input::TranslateNativeEvent(NativeEvent const event, InputEvent& translated)
{
  switch (event.windows.Message_)
  {
  case WM_INPUT:
    return Private::TranslateWin32_WM_INPUT(event, translated);
  }
  return false;
}

bool Input::Private::TranslateWin32_WM_INPUT(NativeEvent const event, InputEvent& translated)
{
  if (GET_RAWINPUT_CODE_WPARAM(event.windows.WParam_) != RIM_INPUT)
    return false;

  HRAWINPUT hraw = (HRAWINPUT)event.windows.LParam_;
  RAWINPUT  rawData{};
  if (0 != GetRawInputData(hraw, RID_INPUT, &rawData, nullptr, sizeof(RAWINPUTHEADER)) || rawData.header.dwType == RIM_TYPEHID)
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
        {Kind::Mouse_Press, MouseEvent::MouseWheel},
        {Kind::Mouse_Press, MouseEvent::MouseWheel},
    };
    const RAWMOUSE& mouse = rawData.data.mouse;
    int const       index = std::countr_zero(mouse.usButtonFlags);
    translated.Kind_      = infos[index].Kind_;

    MouseEvent ev{};
    ev.Button_ = infos[index].Button_;
    ev.WheelY  = mouse.usButtonFlags & RI_MOUSE_WHEEL ? mouse.usButtonData : 0;
    if (mouse.usFlags & MOUSE_MOVE_RELATIVE)
    {
      ev.MoveX_ = mouse.lLastX;
      ev.MoveY_ = mouse.lLastY;
    }
    translated.Mouse_ = ev;
  }
  else
  {
    const RAWKEYBOARD& kbd = rawData.data.keyboard;
    if (kbd.MakeCode == KEYBOARD_OVERRUN_MAKE_CODE || kbd.VKey >= UCHAR_MAX)
      return false;

    KeyboardEvent ev{};
    ev.NativeVirtualKey_ = kbd.VKey;

    const HKL kbdLayout = GetKeyboardLayout(0);

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

    BYTE* kbdState{};
    if (GetKeyboardState(kbdState) && ToUnicodeEx(kbd.VKey, ev.ScanCode_, kbdState, &ev.Char_, 1, GE_BIT(2), kbdLayout) <= 0)
      ev.Char_ = 0;

    constexpr Kind kinds[] = {Kind::Key_Press, Kind::Key_Release};
    translated.Kind_       = kinds[kbd.Flags & 0x1];
    translated.Keyboard_   = ev;
  }
  return true;
}
