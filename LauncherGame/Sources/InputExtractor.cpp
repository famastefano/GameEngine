#include "LauncherGame/Sources/InputExtractor.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <bit>
#include <type_traits>

namespace Launcher
{
constexpr Input::EventTypes& operator|=(Input::EventTypes& in, Input::EventTypes flag)
{
    using type_t = std::underlying_type_t<Input::EventTypes>;
    in           = static_cast<Input::EventTypes>(static_cast<type_t>(in) | static_cast<type_t>(flag));
    return in;
}

InputData extractInput(tagRAWINPUT& raw, Input::EventTypes typeToExtract)
{
    using enum Input::EventTypes;

    InputData input{};
    input.type = static_cast<Input::EventTypes>(0);

    if(raw.header.dwType == RIM_TYPEKEYBOARD)
    {
        input.keyboard = {};

        RAWKEYBOARD& kbd = raw.data.keyboard;

        if(typeToExtract & Keyboard)
        {
            input.type |= Keyboard;
            input.keyboard.action   = kbd.Flags & RI_KEY_BREAK ? ActionType::Release : ActionType::Press;
            input.keyboard.scanCode = kbd.MakeCode;
        }

        if(typeToExtract & Character && kbd.Flags == 0) // Character is valid on on Key Press, not release
        {
            BYTE kbdstate[256] = {};
            if(GetKeyboardState(kbdstate) && ToUnicode(kbd.VKey, kbd.MakeCode, kbdstate, &input.keyboard.character, 1, 0x2) > 0)
                input.type |= Character;
        }
    }
    else if(typeToExtract & Mouse && raw.header.dwType == RIM_TYPEMOUSE)
    {
        RAWMOUSE& ms = raw.data.mouse;
        input.mouse  = {};
        input.type   = Mouse;

        if(ms.lLastX != 0 || ms.lLastY != 0)
        {
            input.mouse.type = MouseType::Movement;
            input.mouse.dx   = int16_t(ms.lLastX);
            input.mouse.dy   = int16_t(ms.lLastY);
        }
        else if(ms.usButtonFlags & (RI_MOUSE_WHEEL | RI_MOUSE_HWHEEL))
        {
            input.mouse.type             = MouseType::Wheel;
            input.mouse.isVerticalScroll = ms.usButtonFlags & RI_MOUSE_WHEEL;
            input.mouse.ticks            = float(int16_t(ms.usButtonData)) / WHEEL_DELTA;
        }
        else if(ms.usButtonFlags)
        {
            // clang-format off
            constexpr uint8_t buttonId[10]{
                1,1,
                2,2,
                3,3,
                4,4,
                5,5,
            };
            constexpr ActionType buttonAction[10]{
                ActionType::Press, ActionType::Release,
                ActionType::Press, ActionType::Release,
                ActionType::Press, ActionType::Release,
                ActionType::Press, ActionType::Release,
                ActionType::Press, ActionType::Release,
            };
            // clang-format on

            int const index    = std::countr_zero(ms.usButtonFlags);
            input.mouse.type   = MouseType::Button;
            input.mouse.button = buttonId[index];
            input.mouse.action = buttonAction[index];
        }
    }

    return input;
}
}