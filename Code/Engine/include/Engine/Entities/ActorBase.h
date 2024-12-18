#pragma once

#include <Core/Container/String.h>
#include <Core/Container/Vector.h>
#include <Core/Reflection/Reflection.h>
#include <Engine/API.h>
#include <Engine/Components/ComponentBase.h>

namespace Engine::Entities
{
class ENGINE_API Actor
{
  Core::Vector<Components::ComponentBase*> Components_;
  Core::String<char> Name_;

public:
  Actor(char const*& Name);
  Actor(Actor const& actor)            = delete;
  Actor& operator=(Actor const& actor) = delete;
  Actor(Actor&& actor)                 = delete;
  Actor& operator=(Actor&& actor)      = delete;
  virtual ~Actor()                     = default;

  [[nodiscard]] Core::StringView<char> Name() const;

  virtual void PreInitialize();
  virtual void PostInitialize();
  virtual void PreDeinitialize();
  virtual void PostDeinitialize();
  virtual void PreTickComponents(f32 DeltaTime);
  void         TickComponents(f32 DeltaTime);
  virtual void PostTickComponents(f32 DeltaTime);

  // TODO: How do I attach a component if I need to initialize them via a config file?
  // TODO: How do I detach a component? I need to query its class somehow?
  // TODO: How do I initialize an Actor from a config? Like a serialized data from file? We might map <ActorClass, ConfigType> and serialize from/to memory via an overloaded Serialize function.
};
} // namespace Engine::Entities
