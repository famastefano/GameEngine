#include <Core/Hash/Hash.h>
#include <xxhash/xxhash.h>

u64 Core::CalculateHash(void const* p, i32 const bytes)
{
  return XXH3_64bits(p, bytes);
}
