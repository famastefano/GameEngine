#pragma once

#include <Core/Definitions.h>

namespace Engine::Serialization
{
#pragma pack(push, 1)
struct SerializationHeader
{
  u32 Magic_   = ('F' << 24) | ('A' << 16) | ('M' << 8) | 'A';
  u32 Version_ = 0;
  u64 TypeID_  = 0;
  u32 Length_  = 0;
};
#pragma pack(pop)
} // namespace Engine::Serialization
