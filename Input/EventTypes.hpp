#pragma once

#include <cstdint>

namespace Input
{
enum EventTypes : uint8_t
{
    // OnKeyUp, OnKeyDown
    Keyboard = 0b0001,

    // OnMouseMove, OnMouseDown, OnMouseUp
    Mouse = 0b0010,

    // OnChar
    Character = 0b0100,

    // Not yet supported
    Gamepad = 0b1000,

    All = 0b1111,
};
}
