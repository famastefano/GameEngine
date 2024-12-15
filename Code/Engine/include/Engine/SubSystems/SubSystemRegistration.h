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

#define GE_REGISTER_SUBSYSTEM(Type, SubSystemType)                                                   \
  static struct AutoRegisterSubSystem_##SubSystemType                                                \
  {                                                                                                  \
    AutoRegisterSubSystem_##SubSystemType()                                                          \
    {                                                                                                \
      Engine::RegisterSubSystemFactoryFn(Type, +[]() -> SubSystem* { return new SubSystemType(); }); \
    }                                                                                                \
  } g_AutoRegisterSubSystem_##SubSystemType##_Instance;
