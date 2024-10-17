#include <Core/Allocator/Allocator.h>
#include <UnitTest/UnitTest.h>

UNIT_TEST_SUITE(Allocator)
{
  UNIT_TEST(GlobalAllocator_Free_NullptrNeverCrashes)
  {
    Core::globalAllocator->Free(nullptr);
  }
  UNIT_TEST(GlobalAllocator_IsCopyable)
  {
    UNIT_TEST_REQUIRE(Core::globalAllocator->IsCopyable());
  }
  UNIT_TEST(GlobalAllocator_IsMovable)
  {
    UNIT_TEST_REQUIRE(Core::globalAllocator->IsMovable());
  }
  UNIT_TEST(GlobalAllocator_IsNotOwnedByTheContainer)
  {
    UNIT_TEST_REQUIRE_FALSE(Core::globalAllocator->OwnedByContainer());
  }
  UNIT_TEST(GlobalAllocator_Alloc_IncreasedSizes)
  {
    for(Core::i32 allocSize = 16; allocSize < 10*1024*1024; allocSize *= 2)
    {
      void* p = Core::globalAllocator->Alloc(allocSize, 8);
      UNIT_TEST_REQUIRE(p);
      Core::globalAllocator->Free(p);
    }
  }
  UNIT_TEST(GlobalAllocator_Alloc_IncreasedAlignment)
  {
    for(Core::i32 align = 1; align < 4096; align *= 2)
    {
      void* p = Core::globalAllocator->Alloc(512, align);
      UNIT_TEST_REQUIRE(p);
      Core::globalAllocator->Free(p);
    }
  }
}
