#pragma once

#include <cstdint>

namespace Input
{
/// <summary>
/// Handles different types of inputs.
/// </summary>
class InputHandler
{
    public:
    virtual ~InputHandler() = default;

    // Returns if the event has been handled or not, thus stopping the propagation to other handlers

// `unreferenced formal parameter`, keeps the parameter names in the API
#pragma warning(disable : 4100)

    virtual bool OnKeyUp(double timestamp, uint16_t scanCode) noexcept
    {
        return false;
    }
    virtual bool OnKeyDown(double timestamp, uint16_t scanCode) noexcept
    {
        return false;
    }

    virtual bool OnMouseUp(double timestamp, uint8_t button) noexcept
    {
        return false;
    }
    virtual bool OnMouseDown(double timestamp, uint8_t button) noexcept
    {
        return false;
    }
    virtual bool OnMouseMove(double timestamp, int16_t dx, int16_t dy) noexcept
    {
        return false;
    }
    virtual bool OnMouseWheel(double timestamp, bool isVerticalScroll, float ticks) noexcept
    {
        return false;
    }

    virtual bool OnCharacter(double timestamp, wchar_t character) noexcept
    {
        return false;
    }

#pragma warning(default : 4100)
};
}