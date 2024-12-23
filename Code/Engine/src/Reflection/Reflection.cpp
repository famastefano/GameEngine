#include <Engine/Reflection/Reflection.h>

namespace Engine
{
ENGINE_API Core::CompactFlatMap<u64, TypeMetaData const*>& GetTypesMetaData()
{
  static Core::CompactFlatMap<u64, TypeMetaData const*> metaData;
  return metaData;
}
} // namespace Engine
