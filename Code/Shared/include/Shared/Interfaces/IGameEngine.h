#pragma once

#include <Core/Container/String.h>
#include <Core/Definitions.h>
#include <Shared/API.h>

namespace Shared
{
class EventBase;

class ISubSystem;

class IAudioSubSystem;
class IECSSubSystem;
class IInputSubSystem;
class INetworkingSubSystem;
class IPhysicsSubSystem;
class IRenderingSubSystem;
class IUISubSystem;

class SHARED_API IGameEngine
{
public:
  virtual ~IGameEngine() = default;

  enum class RunningMode
  {
    Headless,
    Gameplay,
    UI,
  };

  virtual RunningMode GetRunningMode() const                    = 0;
  virtual void        SetRunningMode(RunningMode const NewMode) = 0;

  virtual void Tick(f32 DeltaTime) = 0;

  virtual void QueueEvent(EventBase* Event) = 0;

  // Faster access since those are always present
  virtual IAudioSubSystem*      GetAudioSubSystem() const      = 0;
  virtual IECSSubSystem*        GetECSSubSystem() const        = 0;
  virtual IInputSubSystem*      GetInputSubSystem() const      = 0;
  virtual INetworkingSubSystem* GetNetworkingSubSystem() const = 0;
  virtual IPhysicsSubSystem*    GetPhysicsSubSystem() const    = 0;
  virtual IRenderingSubSystem*  GetRenderingSubSystem() const  = 0;
  virtual IUISubSystem*         GetUISubSystem() const         = 0;

  virtual ISubSystem* GetCustomSubSystem(Core::StringView<char> Name) const = 0;

  virtual void RegisterCustomSubSystem(ISubSystem* SubSystem) = 0;
};
} // namespace Shared
