#pragma once

#include <Core/Definitions.h>
#include <Core/Hash/Hash.h>
#include <Engine/API.h>

namespace Engine
{
struct ENGINE_API EventBase
{
  virtual ~EventBase()      = default;
  virtual u64 GetID() const = 0;
};
} // namespace Engine

/*
 * Example:
 * struct MyCustomEvent : public Engine::EventBase
 * {
 *    GE_IMPLEMENT_EVENT(MyCustomEvent)
 *
 *    MyData Data_;
 * };
 */

#define GE_IMPLEMENT_EVENT(EventType)                                          \
public:                                                                        \
  inline static u64 GetUniqueID()                                              \
  {                                                                            \
    static const u64 id = Core::CalculateHash(#EventType, sizeof(#EventType)); \
    return id;                                                                 \
  }                                                                            \
                                                                               \
  inline u64 GetID() const override                                            \
  {                                                                            \
    return GetUniqueID();                                                      \
  }
