#include <Core/Hash/Hash.h>
#include <Core/Assert/Assert.h>

#include "xxhash.h"

u64 Core::CalculateHash(void const* p, i32 const bytes)
{
  check(!p && bytes == 0 || p && bytes > 0);
  return XXH3_64bits(p, (size_t)bytes);
}
