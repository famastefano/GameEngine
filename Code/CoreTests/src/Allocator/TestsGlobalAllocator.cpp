#include <Core/Allocator/Allocator.h>
#include <UnitTest/UnitTest.h>
#include <Windows.h>

UNIT_TEST_SUITE(Allocator)
{
  UNIT_TEST(GlobalAllocator_Free_NullptrNeverCrashes)
  {
    Core::globalAllocator->Free(nullptr, 0);
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
    for (i32 allocSize = 16; allocSize < 10 * 1'024 * 1'024; allocSize *= 2)
    {
      void* p = Core::globalAllocator->Alloc(allocSize, 8);
      UNIT_TEST_REQUIRE(p);
      SecureZeroMemory(p, allocSize);
      Core::globalAllocator->Free(p, 8);
    }
  }
  UNIT_TEST(GlobalAllocator_Alloc_IncreasedAlignment)
  {
    for (i32 align = 1; align < 4'096; align *= 2)
    {
      void* p = Core::globalAllocator->Alloc(align * 2, align);
      UNIT_TEST_REQUIRE(p);
      ((u8*)p)[align * 2 - 1] = 0xFF;
      SecureZeroMemory(p, align * 2);
      Core::globalAllocator->Free(p, align);
    }
  }
  UNIT_TEST(GlobalAllocator_Alloc_OverAlignmentQuantity)
  {
    for (i32 iterations = 0; iterations < 1'024; ++iterations)
    {
      void* p = Core::globalAllocator->Alloc(iterations, 32);
      UNIT_TEST_REQUIRE(p);
      SecureZeroMemory(p, iterations);
      Core::globalAllocator->Free(p, 32);
    }
  }
}
