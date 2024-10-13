#pragma once

#undef GAME_ENGINE_RTTI

#define GAME_ENGINE_DEFINE_RTTI(Type)                    \
public:                                                  \
  static ::Core::Reflection::RTTI const& GetRTTI()       \
  {                                                      \
    static ::Core::Reflection::RTTI rtti{.Name = #Type}; \
    return rtti;                                         \
  }                                                      \
                                                         \
  template <typename T>                                  \
  T* Cast()                                              \
  {                                                      \
    if (&GetRTTI() == &T::GetRTTI())                     \
      return static_cast<T*>(this);                      \
    return nullptr;                                      \
  }                                                      \
                                                         \
private:

namespace Core::Reflection
{
struct RTTI
{
  char const* Name;
};
} // namespace Core::Reflection
