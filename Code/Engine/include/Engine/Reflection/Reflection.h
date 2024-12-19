#pragma once

#include <Core/Container/FlatMap.h>
#include <Core/Container/Span.h>
#include <Core/Container/Vector.h>
#include <Engine/Serialization/Serialization.h>
#include <concepts>

namespace Engine
{
struct TypeMetaData
{
  using FactoryFn     = void* (*)();
  using SerializeFn   = void (*)(void*, u32&, Core::Vector<u8>&);
  using DeserializeFn = void (*)(void*, Serialization::SerializationHeader const&, Core::Span<u8 const>);

  u64           ID_{};
  char const*   Name_{};
  FactoryFn     Factory_{};
  SerializeFn   Serialize_{};
  DeserializeFn Deserialize_{};
};

template <typename T>
concept Reflectable = requires(T t) {
  { t.GetTypeMetaData() } -> std::same_as<TypeMetaData const&>;
  { T::GetStaticTypeMetaData() } -> std::same_as<TypeMetaData const&>;
};

template <typename T>
concept Serializable = requires(T t) {
  t.Serialize(std::declval<u32>, std::declval<Core::Vector<u8>>);
  t.Deserialize(std::declval<Serialization::SerializationHeader const>, std::declval<Core::Span<u8 const>>);
};

template <typename T>
TypeMetaData MakeMetaData(u64 const ID, char const* Name)
{
  return {
      .ID_      = ID,
      .Name_    = Name,
      .Factory_ = +[]() -> void* { return new T(); },
  };
}

template <Serializable T>
TypeMetaData MakeMetaData(u64 const ID, char const* Name)
{
  return {
      .ID_          = ID,
      .Name_        = Name,
      .Factory_     = +[]() -> void* { return new T(); },
      .Serialize_   = +[](void* Instance, void* Data) { ((T*)Instance)->Serialize(Data); },
      .Deserialize_ = +[](void* Instance, void* Data) { ((T*)Instance)->Deserialize(Data); },
  };
}

extern Core::FlatMap<u64, TypeMetaData*> TypeMetaDatas;

template <typename T>
struct AutoRegisterTypeMetadata
{
  explicit AutoRegisterTypeMetadata(TypeMetaData& MetaData)
  {
    TypeMetaDatas.TryEmplace(MetaData.ID_, &MetaData);
  }
};
} // namespace Engine

#define GE_DECLARE_STRUCT_TYPE_METADATA_BASE()                                   \
  [[nodiscard]] virtual const Engine::TypeMetaData& GetTypeMetaData() const = 0; \
                                                                                 \
  template <Engine::Reflectable T>                                               \
  [[nodiscard]] T* GetAs()                                                       \
  {                                                                              \
    if (GetTypeMetaData().ID_ == T::GetStaticTypeMetaData().ID_)                 \
      return static_cast<T*>(this);                                              \
    return nullptr;                                                              \
  }

#define GE_DECLARE_CLASS_TYPE_METADATA_BASE()                                    \
public:                                                                          \
  [[nodiscard]] virtual const Engine::TypeMetaData& GetTypeMetaData() const = 0; \
                                                                                 \
  template <Engine::Reflectable T>                                 \
  [[nodiscard]] T* GetAs()                                                       \
  {                                                                \
    if (GetTypeMetaData().ID_ == T::GetStaticTypeMetaData().ID_)                 \
      return static_cast<T*>(this);                                              \
    return nullptr;                                                              \
  }                                                                              \
                                                                                 \
private:

#define GE_DECLARE_CLASS_TYPE_METADATA()                       \
public:                                                        \
  [[nodiscard]] virtual const Engine::TypeMetaData& GetTypeMetaData() const; \
  [[nodiscard]] static const Engine::TypeMetaData&  GetStaticTypeMetaData(); \
                                                               \
private:

#define GE_DECLARE_STRUCT_TYPE_METADATA()                      \
  [[nodiscard]] virtual const Engine::TypeMetaData& GetTypeMetaData() const; \
  [[nodiscard]] static const Engine::TypeMetaData&  GetStaticTypeMetaData();

#define GE_DEFINE_TYPE_METADATA(FullyQualifiedTypeWithNamespace)                                                                    \
  const Engine::TypeMetaData& FullyQualifiedTypeWithNamespace::GetTypeMetaData() const                                              \
  {                                                                                                                                 \
    return GetStaticTypeMetaData();                                                                                                 \
  }                                                                                                                                 \
  const Engine::TypeMetaData& FullyQualifiedTypeWithNamespace::GetStaticTypeMetaData()                                              \
  {                                                                                                                                 \
    static const char*                    name         = #FullyQualifiedTypeWithNamespace;                                          \
    static Engine::TypeMetaData           typeMetaData = Engine::MakeMetaData<FullyQualifiedTypeWithNamespace>((u64) & name, name); \
    static AutoRegisterTypeMetadata<void> reg{typeMetaData};                                                                        \
    return typeMetaData;                                                                                                            \
  }
