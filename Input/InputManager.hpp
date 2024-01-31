#pragma once

#include "Input/EventTypes.hpp"

#include <vector>

namespace Input
{
class InputHandler;

class InputManager
{
    public:
    enum class HandlerPriority : int32_t
    {
        Low,
        Normal,
        High,
    };

    private:
    struct Handler
    {
        InputHandler*   hnd;
        HandlerPriority priority;
    };

    using HandlerCollection = std::vector<Handler>;

    HandlerCollection keyHandlers;
    HandlerCollection mouseHandlers;
    HandlerCollection charHandlers;

    uint8_t canHandleEvent = 0;

    HandlerCollection::const_iterator findHandler(EventTypes events, InputHandler& handler) const noexcept;

    public:
    static InputManager& instance() noexcept;

    void registerHandler(EventTypes events, InputHandler& handler, HandlerPriority priority = HandlerPriority::Normal) noexcept;
    void unregisterHandler(EventTypes events, InputHandler& handler) noexcept;

    bool canHandleKeyboard() const noexcept;
    bool canHandleMouse() const noexcept;
    bool canHandleCharacters() const noexcept;

    void dispatchKeyUp(double timestamp, uint16_t scanCode);
    void dispatchKeyDown(double timestamp, uint16_t scanCode);

    void dispatchMouseUp(double timestamp, uint8_t button);
    void dispatchMouseDown(double timestamp, uint8_t button);
    void dispatchMouseMove(double timestamp, int16_t dx, int16_t dy);
    void dispatchMouseWheel(double timestamp, bool isVerticalScroll, float ticks);

    void dispatchCharacter(double timestamp, wchar_t character);
};
}
