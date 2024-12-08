#pragma once

#include <Core/Container/String.h>
#include <Core/Definitions.h>
#include <Engine/API.h>

namespace Engine
{
class ENGINE_API SubSystem
{
public:
  virtual ~SubSystem() = default;

  virtual void Tick(f32 DeltaTime) = 0;

  virtual Core::StringView<char> Name() const = 0;
};
} // namespace Engine

#define GE_IMPLEMENT_SUBSYSTEM(SubSystemName)         \
public:                                               \
  inline Core::StringView<char> Name() const override \
  {                                                   \
    return #SubSystemName;                            \
  }                                                   \
                                                      \
private:
