#pragma once

#include <Engine/Events/EventBase.h>

namespace Engine
{
enum class Kind : u8
{
  Mouse_Press,
  Mouse_Release,
  Mouse_Move,
  Mouse_Wheel,
  Key_Press,
  Key_Release,
};

struct MouseEvent
{
  i16 MoveX_, MoveY_;
  i16 WheelY;
  enum Button : i16
  {
    NoButton,
    Mouse0,
    Mouse1,
    Mouse2,
    Mouse3,
    Mouse4,
  } Button_;
};

struct KeyboardEvent
{
  u32     ScanCode_;
  u16     NativeVirtualKey_;
  wchar_t Char_;
};

struct ENGINE_API EventInput : EventBase
{
  GE_DECLARE_STRUCT_TYPE_METADATA()

  union {
    MouseEvent    Mouse_;
    KeyboardEvent Keyboard_;
  };
  Kind Kind_;
};
} // namespace Input
