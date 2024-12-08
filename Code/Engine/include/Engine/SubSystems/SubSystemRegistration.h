#pragma once

#include <Engine/API.h>

namespace Engine
{
class SubSystem;

using SubSystemFactoryFn = SubSystem* (*)();

enum class SubSystemType
{
  Engine,
  World,
  Player,

  Count,
};

ENGINE_API void RegisterSubSystemFactoryFn(SubSystemType const Type, SubSystemFactoryFn Fn);
} // namespace Engine

#define GE_REGISTER_SUBSYSTEM(Type, SubSystem)                                               \
  struct AutoRegisterSubSystem_##SubSystem                                                   \
  {                                                                                          \
    AutoRegisterSubSystem_##SubSystem()                                                      \
    {                                                                                        \
      Engine::RegisterSubSystemFactoryFn(#SubSystem, Type, +[] { return new SubSystem(); }); \
    }                                                                                        \
  } g_AutoRegisterSubSystem_##SubSystem##_Instance;
