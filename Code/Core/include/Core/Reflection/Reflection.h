#pragma once

#include <Core/Definitions.h>
#include <concepts>

namespace Core
{
struct TypeMetaData
{
  u64         ID_;
  char const* Name_;
};
template <typename T>
concept Reflectable = requires(T t) {
  { t.GetTypeMetaData() } -> std::same_as<TypeMetaData const&>;
  { T::GetStaticTypeMetaData() } -> std::same_as<TypeMetaData const&>;
};
} // namespace Core

#define GE_DECLARE_TYPE_METADATA_BASE()                          \
  virtual const Core::TypeMetaData& GetTypeMetaData() const = 0; \
                                                                 \
  template <Core::Reflectable T>                                 \
  T* GetAs()                                                     \
  {                                                              \
    if (GetTypeMetaData().ID_ == T::GetStaticTypeMetaData().ID_) \
      return static_cast<T*>(this);                              \
    return nullptr;                                              \
  }

#define GE_DECLARE_CLASS_TYPE_METADATA()                     \
public:                                                      \
  virtual const Core::TypeMetaData& GetTypeMetaData() const; \
                                                             \
private:

#define GE_DECLARE_STRUCT_TYPE_METADATA()                    \
  virtual const Core::TypeMetaData& GetTypeMetaData() const; \
  static const Core::TypeMetaData&  GetStaticTypeMetaData();

#define GE_DEFINE_TYPE_METADATA(FullyQualifiedTypeWithNamespace)                     \
  const Core::TypeMetaData& FullyQualifiedTypeWithNamespace::GetTypeMetaData() const \
  {                                                                                  \
    return GetStaticTypeMetaData();                                                  \
  }                                                                                  \
  const Core::TypeMetaData& FullyQualifiedTypeWithNamespace::GetStaticTypeMetaData() \
  {                                                                                  \
    static const char*        name = #FullyQualifiedTypeWithNamespace;               \
    static Core::TypeMetaData typeMetaData{                                          \
        .ID_   = (u64) & name,                                                       \
        .Name_ = name,                                                               \
    };                                                                               \
    return typeMetaData;                                                             \
  }
