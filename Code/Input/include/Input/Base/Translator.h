#pragma once

#include <Input/API.h>
#include <Input/Base/Events.h>

namespace Input
{
struct Win32NativeEvent
{
  void* Hwnd_;
  u64   WParam_;
  i64   LParam_;
  u32   Message_;
};

struct NativeEvent
{
  union {
    Win32NativeEvent windows;
  };
};

INPUT_API bool TranslateNativeEvent(NativeEvent const event, InputEvent& translated);
} // namespace Input
