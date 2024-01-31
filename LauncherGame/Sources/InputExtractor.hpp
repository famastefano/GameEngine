#pragma once

#include "Input/EventTypes.hpp"

#include <cstdint>

struct tagRAWINPUT;

namespace Launcher
{
enum class ActionType : int8_t
{
    Press,
    Release
};

enum class MouseType : int8_t
{
    Button,
    Movement,
    Wheel
};

struct MouseInput
{
    float      ticks;
    int16_t    dx, dy;
    MouseType  type;
    uint8_t    button;
    ActionType action;
    bool       isVerticalScroll;
};

struct KeyboardInput
{
    wchar_t    character;
    uint16_t   scanCode;
    ActionType action;
};

struct InputData
{
    Input::EventTypes type;
    union
    {
        MouseInput    mouse;
        KeyboardInput keyboard;
    };
};

InputData extractInput(tagRAWINPUT& raw, Input::EventTypes typeToExtract);
}
