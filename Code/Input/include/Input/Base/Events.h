#pragma once

#include <Core/Definitions.h>

namespace Input
{
enum class Kind : u8
{
  Mouse_Press,
  Mouse_Release,
  Key_Press,
  Key_Release
};

struct MouseEvent
{
  i16 MoveX_, MoveY_;
  i16 WheelY;
  enum Button : i16
  {
    Mouse0,
    Mouse1,
    Mouse2,
    Mouse3,
    Mouse4,
    MouseWheel,
  } Button_;
};

struct KeyboardEvent
{
  u32     ScanCode_;
  u16     NativeVirtualKey_;
  wchar_t Char_;
};

struct InputEvent
{
  union {
    MouseEvent    Mouse_;
    KeyboardEvent Keyboard_;
  };
  Kind Kind_;
};
} // namespace Input
