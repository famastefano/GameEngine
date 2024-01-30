#include "Input/InputHandler.hpp"
#include "Input/InputManager.hpp"
#include "TestCommon/CommonMacros.hpp"

#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <map>

using Input::InputHandler;
using Input::InputManager;

struct CountHandler : public InputHandler
{
    int OnKeyUpCounter      = 0;
    int OnKeyDownCounter    = 0;
    int OnMouseUpCounter    = 0;
    int OnMouseDownCounter  = 0;
    int OnMouseMoveCounter  = 0;
    int OnMouseWheelCounter = 0;
    int OnCharacterCounter  = 0;

    virtual bool OnKeyUp(double, uint16_t)
    {
        ++OnKeyUpCounter;
        return false;
    }
    virtual bool OnKeyDown(double, uint16_t)
    {
        ++OnKeyDownCounter;
        return false;
    }

    virtual bool OnMouseUp(double, uint8_t)
    {
        ++OnMouseUpCounter;
        return false;
    }
    virtual bool OnMouseDown(double, uint8_t)
    {
        ++OnMouseDownCounter;
        return false;
    }
    virtual bool OnMouseMove(double, int16_t, int16_t)
    {
        ++OnMouseMoveCounter;
        return false;
    }
    virtual bool OnMouseWheel(double, bool, float)
    {
        ++OnMouseWheelCounter;
        return false;
    }

    virtual bool OnCharacter(double, wchar_t)
    {
        ++OnCharacterCounter;
        return false;
    }
};

struct ToggleHandler : public InputHandler
{
    bool handlesAll       = false;
    bool handlesKeyboard  = false;
    bool handlesMouse     = false;
    bool handlesCharacter = false;

    virtual bool OnKeyUp(double, uint16_t)
    {
        return handlesAll || handlesKeyboard;
    }
    virtual bool OnKeyDown(double, uint16_t)
    {
        return handlesAll || handlesKeyboard;
    }

    virtual bool OnMouseUp(double, uint8_t)
    {
        return handlesAll || handlesMouse;
    }
    virtual bool OnMouseDown(double, uint8_t)
    {
        return handlesAll || handlesMouse;
    }
    virtual bool OnMouseMove(double, int16_t, int16_t)
    {
        return handlesAll || handlesMouse;
    }
    virtual bool OnMouseWheel(double, bool, float)
    {
        return handlesAll || handlesMouse;
    }

    virtual bool OnCharacter(double, wchar_t)
    {
        return handlesAll || handlesCharacter;
    }
};

struct BlackHoleHandler : public InputHandler
{
    virtual bool OnKeyUp(double, uint16_t)
    {
        return true;
    }
    virtual bool OnKeyDown(double, uint16_t)
    {
        return true;
    }

    virtual bool OnMouseUp(double, uint8_t)
    {
        return true;
    }
    virtual bool OnMouseDown(double, uint8_t)
    {
        return true;
    }
    virtual bool OnMouseMove(double, int16_t, int16_t)
    {
        return true;
    }
    virtual bool OnMouseWheel(double, bool, float)
    {
        return true;
    }

    virtual bool OnCharacter(double, wchar_t)
    {
        return true;
    }
};

TEST_CASE("The InputManager", "[input]")
{
    /*
     * Can Handle X returns false:
     * 1. Newly constructed manager
     * 2. Adds 1+ handlers for other events
     * 3. Removes all the handlers for that event
     * 
     * Can Handle X returns true:
     * 1. If at least 1 handler for that event is present
     * 
     * Dispatching:
     * 1. No handler = no crash
     * 2. single Handler sees that event
     * 3. follows priority (registers multiple handlers)
     */

    InputManager     manager;
    CountHandler     countHandler;
    ToggleHandler    toggleHandler;
    BlackHoleHandler blackHoleHandler;

    SECTION("Can events be handled?")
    {
        SECTION("No, if no handler has been registered")
        {
            GE_TEST_TRUE(!manager.canHandleCharacters());
            GE_TEST_TRUE(!manager.canHandleKeyboard());
            GE_TEST_TRUE(!manager.canHandleMouse());
        }

        SECTION("Yes, if an handler has been registered")
        {
            SECTION("But only for that specific type")
            {
                std::array<Input::EventTypes, 3> events{
                    Input::Character,
                    Input::Keyboard,
                    Input::Mouse,
                };

                using can_handle_fn_t = bool (InputManager::*)() const noexcept;

                std::map<Input::EventTypes, can_handle_fn_t> canHandleMap{
                    {Input::Character, &InputManager::canHandleCharacters},
                    {Input::Keyboard, &InputManager::canHandleKeyboard},
                    {Input::Mouse, &InputManager::canHandleMouse},
                };

                for(std::size_t i = 0; i < events.size(); ++i)
                {
                    Input::EventTypes registeredEvent = events[0];
                    manager.registerHandler(registeredEvent, countHandler);

                    can_handle_fn_t check0 = canHandleMap[events[0]];
                    can_handle_fn_t check1 = canHandleMap[events[1]];
                    can_handle_fn_t check2 = canHandleMap[events[2]];

                    GE_TEST_TRUE((manager.*check0)());
                    GE_TEST_TRUE(!(manager.*check1)());
                    GE_TEST_TRUE(!(manager.*check2)());

                    manager.unregisterHandler(registeredEvent, countHandler);
                    auto begin = events.begin();
                    auto end   = events.end();
                    std::rotate(begin, begin + 1, end);
                }
            }
        }

        SECTION("No, if the all the handlers have been unregistered")
        {
            CountHandler handler1, handler2;
            manager.registerHandler(Input::Mouse, handler1);
            manager.registerHandler(Input::Mouse, handler2);
            GE_TEST_TRUE(manager.canHandleMouse());

            manager.unregisterHandler(Input::Mouse, handler1);
            GE_TEST_TRUE(manager.canHandleMouse());

            manager.unregisterHandler(Input::Mouse, handler2);
            GE_TEST_TRUE(!manager.canHandleMouse());
        }
    }

    SECTION("Dispatching")
    {
        SECTION("Without any handler, doesn't crash")
        {
            manager.dispatchKeyUp(0, 0);
            manager.dispatchKeyDown(0, 0);
            manager.dispatchMouseUp(0, 0);
            manager.dispatchMouseDown(0, 0);
            manager.dispatchMouseMove(0, 0, 0);
            manager.dispatchMouseWheel(0, true, 0);
            manager.dispatchCharacter(0, L'a');
            SUCCEED();
        }

        SECTION("With an registered handler, it is dispatched")
        {
            manager.registerHandler(Input::Character, countHandler);
            manager.dispatchCharacter(0, L'a');
            GE_TEST_TRUE(countHandler.OnCharacterCounter == 1);
        }

        SECTION("With multiple handlers, dispatching is interrupted as soon as the event is handled")
        {
            // Same priority but in LIFO order
            manager.registerHandler(Input::All, countHandler);
            manager.registerHandler(Input::All, blackHoleHandler);
            manager.dispatchCharacter(0, L'a');
            GE_TEST_TRUE(countHandler.OnCharacterCounter == 0);
        }

        SECTION("With multiple handlers, the one with the top priority has the event dispatched before the others")
        {
            manager.registerHandler(Input::All, blackHoleHandler, InputManager::HandlerPriority::High);
            manager.registerHandler(Input::All, countHandler);
            manager.dispatchCharacter(0, L'a');
            GE_TEST_TRUE(countHandler.OnCharacterCounter == 0);
        }

        SECTION("With multiple handlers, they are inserted ordered by priority")
        {
            toggleHandler.handlesKeyboard = true;

            manager.registerHandler(Input::All, blackHoleHandler, InputManager::HandlerPriority::Low);
            manager.registerHandler(Input::All, toggleHandler, InputManager::HandlerPriority::High);
            manager.registerHandler(Input::All, countHandler, InputManager::HandlerPriority::Normal);

            manager.dispatchCharacter(0, L'a');
            GE_TEST_TRUE(countHandler.OnCharacterCounter == 1);

            manager.dispatchKeyUp(0, 0);
            GE_TEST_TRUE(countHandler.OnKeyUpCounter == 0);

            manager.dispatchMouseUp(0, 0);
            GE_TEST_TRUE(countHandler.OnMouseUpCounter == 1);
        }

        SECTION("With multiple handlers, they are inserted in LIFO order if others with the same priority are present")
        {
            toggleHandler.handlesAll = true;
            
            manager.registerHandler(Input::Character, toggleHandler, InputManager::HandlerPriority::High);
            manager.registerHandler(Input::Character, countHandler, InputManager::HandlerPriority::Normal);

            manager.dispatchCharacter(0, L'0');
            GE_TEST_TRUE(countHandler.OnCharacterCounter == 0);

            manager.unregisterHandler(Input::Character, countHandler);
            manager.registerHandler(Input::Character, countHandler, InputManager::HandlerPriority::High);

            manager.dispatchCharacter(0, L'0');
            GE_TEST_TRUE(countHandler.OnCharacterCounter == 1);
        }
    }
}
