#include "Input/InputManager.hpp"

#include "Core/Core.hpp"
#include "Input/InputHandler.hpp"
#include "Input/Sources/LogInput.hpp"

#include <algorithm>

namespace Input
{
static constexpr bool operator<(InputManager::HandlerPriority pa, InputManager::HandlerPriority pb)
{
    return static_cast<int>(pa) < static_cast<int>(pb);
}

template<typename Collection, typename MemberFn, typename... Args>
static void dispatch(Collection& coll, MemberFn fn, Args&&... args)
{
    for(auto& handler: coll)
        if((handler.hnd->*fn)(std::forward<Args>(args)...))
            return;
}

static constexpr auto findHandlerPredicate(InputHandler const& handler)
{
    return [p = &handler](auto const& h)
    {
        return h.hnd == p;
    };
}

static constexpr auto findHandlerPrioritySpot(InputManager::HandlerPriority priority)
{
    return [priority](auto const& h)
    {
        return h.priority == priority || h.priority < priority;
    };
}

InputManager::HandlerCollection::const_iterator InputManager::findHandler(EventTypes events, InputHandler& handler) const noexcept
{
    using std::ranges::find_if;

    if(events & Keyboard)
        return find_if(keyHandlers, findHandlerPredicate(handler));
    if(events & Mouse)
        return find_if(mouseHandlers, findHandlerPredicate(handler));
    if(events & Character)
        return find_if(charHandlers, findHandlerPredicate(handler));

    assertNoEntry();
}
InputManager& InputManager::instance() noexcept
{
    static InputManager manager;
    return manager;
}
void InputManager::registerHandler(EventTypes events, InputHandler& handler, HandlerPriority priority) noexcept
{
    using std::ranges::end;
    using std::ranges::find_if;

    assertf((events & All) != 0, L"Invalid event enum `{:#b}`", static_cast<uint8_t>(events));
    assertf(priority == HandlerPriority::Low || priority == HandlerPriority::Normal || priority == HandlerPriority::High, L"Invalid priority.");

    Handler newHandler{&handler, priority};

    if(events & Keyboard)
    {
        verifyf(findHandler(Keyboard, handler) == end(keyHandlers), L"Keyboard handler already registered");
        keyHandlers.insert(find_if(keyHandlers, findHandlerPrioritySpot(priority)), newHandler);
    }

    if(events & Mouse)
    {
        verifyf(findHandler(Mouse, handler) == end(mouseHandlers), L"Mouse handler already registered");
        mouseHandlers.insert(find_if(mouseHandlers, findHandlerPrioritySpot(priority)), newHandler);
    }

    if(events & Character)
    {
        verifyf(findHandler(Character, handler) == end(charHandlers), L"Character handler already registered");
        charHandlers.insert(find_if(charHandlers, findHandlerPrioritySpot(priority)), newHandler);
    }

    canHandleEvent |= events;
}
void InputManager::unregisterHandler(EventTypes events, InputHandler& handler) noexcept
{
    using std::ranges::end;

    assertf((events & All) != 0, L"Invalid event enum `{:#b}`", static_cast<uint8_t>(events));

    if(events & Keyboard)
    {
        if(auto it = findHandler(Keyboard, handler); it != end(keyHandlers))
        {
            keyHandlers.erase(it);
            if(keyHandlers.empty())
                canHandleEvent &= ~Keyboard;
        }
        else
        {
            LOG(LogInput, Core::LogLevel::Warning, L"Keyboard handler has never been registered.");
        }
    }

    if(events & Mouse)
    {
        if(auto it = findHandler(Mouse, handler); it != end(mouseHandlers))
        {
            mouseHandlers.erase(it);
            if(mouseHandlers.empty())
                canHandleEvent &= ~Mouse;
        }
        else
        {
            LOG(LogInput, Core::LogLevel::Warning, L"Mouse handler has never been registered.");
        }
    }

    if(events & Character)
    {
        if(auto it = findHandler(Character, handler); it != end(charHandlers))
        {
            charHandlers.erase(it);
            if(mouseHandlers.empty())
                canHandleEvent &= ~Character;
        }
        else
        {
            LOG(LogInput, Core::LogLevel::Warning, L"Character handler has never been registered.");
        }
    }
}
bool InputManager::canHandleKeyboard() const noexcept
{
    return canHandleEvent & Keyboard;
}
bool InputManager::canHandleMouse() const noexcept
{
    return canHandleEvent & Mouse;
}
bool InputManager::canHandleCharacters() const noexcept
{
    return canHandleEvent & Character;
}
void InputManager::dispatchKeyUp(double timestamp, uint16_t scanCode)
{
    expectf(canHandleKeyboard(), L"Key Up event won't be handled because no keyboard handler were registered.");
    dispatch(keyHandlers, &InputHandler::OnKeyUp, timestamp, scanCode);
}
void InputManager::dispatchKeyDown(double timestamp, uint16_t scanCode)
{
    expectf(canHandleKeyboard(), L"Key Down event won't be handled because no keyboard handler were registered.");
    dispatch(keyHandlers, &InputHandler::OnKeyDown, timestamp, scanCode);
}
void InputManager::dispatchMouseUp(double timestamp, uint8_t button)
{
    expectf(canHandleMouse(), L"Mouse up event won't be handled because no mouse handler were registered.");
    dispatch(mouseHandlers, &InputHandler::OnMouseUp, timestamp, button);
}
void InputManager::dispatchMouseDown(double timestamp, uint8_t button)
{
    expectf(canHandleMouse(), L"Mouse down event won't be handled because no mouse handler were registered.");
    dispatch(mouseHandlers, &InputHandler::OnMouseDown, timestamp, button);
}
void InputManager::dispatchMouseMove(double timestamp, int16_t dx, int16_t dy)
{
    expectf(canHandleMouse(), L"Mouse move event won't be handled because no mouse handler were registered.");
    dispatch(mouseHandlers, &InputHandler::OnMouseMove, timestamp, dx, dy);
}
void InputManager::dispatchMouseWheel(double timestamp, bool isVerticalScroll, float ticks)
{
    expectf(canHandleMouse(), L"Mouse wheel event won't be handled because no mouse handler were registered.");
    dispatch(mouseHandlers, &InputHandler::OnMouseWheel, timestamp, isVerticalScroll, ticks);
}
void InputManager::dispatchCharacter(double timestamp, wchar_t character)
{
    expectf(canHandleCharacters(), L"Character event won't be handled because no character handler were registered.");
    dispatch(charHandlers, &InputHandler::OnCharacter, timestamp, character);
}
}