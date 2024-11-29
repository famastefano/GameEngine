#pragma once

#include <Core/Definitions.h>
#include <Shared/API.h>

namespace Shared
{
class SHARED_API EventBase
{
protected:
  static i32 GenerateUniqueEventID();

public:
  virtual ~EventBase() = default;

  virtual i32 GetID() const = 0;
};
} // namespace Shared

/*
 * Example:
 * struct MyCustomEvent : public Shared::EventBase
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
