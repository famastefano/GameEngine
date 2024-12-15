#pragma once

#include <Engine/API.h>
#include <Engine/Events/Input/InputEvents.h>

namespace Engine
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
    Win32NativeEvent Windows_;
  };
};

ENGINE_API bool TranslateNativeEvent(NativeEvent const event, InputEvent& Translated);
} // namespace Input
