#pragma once

#include <Core/Definitions.h>
#include <Engine/API.h>

namespace Engine
{
class ENGINE_API EventBase
{
protected:
  static i32 GenerateUniqueEventID();

public:
  virtual ~EventBase() = default;

  virtual i32 GetID() const = 0;
};
} // namespace Engine

/*
 * Example:
 * struct MyCustomEvent : public Engine::EventBase
 * {
 *    GE_IMPLEMENT_EVENT()
 *
 *    MyData Data_;
 * };
 */

#define GE_IMPLEMENT_EVENT()                 \
public:                                      \
  inline static i32 GetUniqueID() const      \
  {                                          \
    static i32 id = GenerateUniqueEventID(); \
    return id;                               \
  }                                          \
                                             \
  inline i32 GetID() const override          \
  {                                          \
    return GetUniqueID();                    \
  }
