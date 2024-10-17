#include <Core/Allocator/Allocator.h>
#include <UnitTest/UnitTest.h>

UNIT_TEST_SUITE(Allocator)
{
  UNIT_TEST(GlobalAllocator_Free_NullptrNeverCrashes)
  {
    Core::globalAllocator->Free(nullptr);
  }
}
