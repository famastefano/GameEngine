#pragma once

#include <Core/Container/String.h>
#include <Core/Definitions.h>
#include <Shared/API.h>

namespace Shared
{
class SHARED_API ISubSystem
{
public:
  virtual ~ISubSystem() = default;

  virtual void Tick(f32 DeltaTime) = 0;

  virtual Core::StringView<char> Name() const = 0;
};
} // namespace Shared

#define GE_IMPLEMENT_SUBSYSTEM(SubSystemName)         \
public:                                               \
  inline Core::StringView<char> Name() const override \
  {                                                   \
    return #SubSystemName;                            \
  }                                                   \
                                                      \
private:
