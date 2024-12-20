#pragma once

#include <Core/Container/FlatMap.h>
#include <Core/Container/Span.h>
#include <Core/Container/Vector.h>
#include <Engine/API.h>
#include <Engine/Serialization/Serialization.h>
#include <concepts>

namespace Engine
{
struct TypeMetaData
{
  enum Kind : u16
  {
    Engine,
    EngineSubSystem,
    WorldSubSystem,
    PlayerSubSystem,
    Actor,
    Component,
    Event,

    // For new types not of the Engine module (ie. Game etc.), you can use Kind values in the range [Custom, max(u16)]
    Custom = 32'767,
  };

  using FactoryFn     = void* (*)();
  using SerializeFn   = void (*)(void*, u32&, Core::Vector<u8>&);
  using DeserializeFn = void (*)(void*, Serialization::SerializationHeader const&, Core::Span<u8 const>);

  // Since this might be serialized, we version the metadata also.
  u16 const     Version_ = 0;
  u16           Kind_{};
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
TypeMetaData MakeMetaData(u64 const ID, char const* Name, u16 const Kind)
{
  return {
      .Kind_    = Kind,
      .ID_      = ID,
      .Name_    = Name,
      .Factory_ = +[]() -> void* { return new T(); },
  };
}

template <Serializable T>
TypeMetaData MakeMetaData(u64 const ID, char const* Name, u16 const Kind)
{
  return {
      .Kind_        = Kind,
      .ID_          = ID,
      .Name_        = Name,
      .Factory_     = +[]() -> void* { return new T(); },
      .Serialize_   = +[](void* Instance, void* Data) { ((T*)Instance)->Serialize(Data); },
      .Deserialize_ = +[](void* Instance, void* Data) { ((T*)Instance)->Deserialize(Data); },
  };
}

ENGINE_API extern Core::FlatMap<u64, TypeMetaData const*> TypeMetaDatas;

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
  template <Engine::Reflectable T>                                               \
  [[nodiscard]] T* GetAs()                                                       \
  {                                                                              \
    if (GetTypeMetaData().ID_ == T::GetStaticTypeMetaData().ID_)                 \
      return static_cast<T*>(this);                                              \
    return nullptr;                                                              \
  }                                                                              \
                                                                                 \
private:

#define GE_DECLARE_CLASS_TYPE_METADATA()                                     \
public:                                                                      \
  [[nodiscard]] virtual const Engine::TypeMetaData& GetTypeMetaData() const; \
  [[nodiscard]] static const Engine::TypeMetaData&  GetStaticTypeMetaData(); \
                                                                             \
private:

#define GE_DECLARE_STRUCT_TYPE_METADATA()                                    \
  [[nodiscard]] virtual const Engine::TypeMetaData& GetTypeMetaData() const; \
  [[nodiscard]] static const Engine::TypeMetaData&  GetStaticTypeMetaData();

#define GE_DEFINE_TYPE_METADATA(FullyQualifiedTypeWithNamespace, TypeKind)                                                                    \
  const Engine::TypeMetaData& FullyQualifiedTypeWithNamespace::GetTypeMetaData() const                                                        \
  {                                                                                                                                           \
    return GetStaticTypeMetaData();                                                                                                           \
  }                                                                                                                                           \
  const Engine::TypeMetaData& FullyQualifiedTypeWithNamespace::GetStaticTypeMetaData()                                                        \
  {                                                                                                                                           \
    static const char*                    name         = #FullyQualifiedTypeWithNamespace;                                                    \
    static Engine::TypeMetaData           typeMetaData = Engine::MakeMetaData<FullyQualifiedTypeWithNamespace>((u64) & name, name, TypeKind); \
    static AutoRegisterTypeMetadata<void> reg{typeMetaData};                                                                                  \
    return typeMetaData;                                                                                                                      \
  }
