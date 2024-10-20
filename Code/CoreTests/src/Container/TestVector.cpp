#include <Core/Container/Vector.h>
#include <UnitTest/UnitTest.h>

UNIT_TEST_SUITE(Container)
{
  using Core::Vector;

  struct NullAllocator : Core::IAllocator
  {
    __declspec(allocator) __declspec(restrict) void* Alloc(i64 const size, i32 const alignment)
    {
      return nullptr;
    }
    __declspec(allocator) __declspec(restrict) __declspec(noalias) void* Realloc(void* p, i64 const size, i32 const alignment)
    {
      return nullptr;
    }
    __declspec(noalias) void Free(void* p, i32 const alignment)
    {
    }
    bool IsMovable()
    {
      return false;
    }
    bool IsCopyable()
    {
      return false;
    }
    bool OwnedByContainer()
    {
      return false;
    }
  };

  UNIT_TEST(Vector_TrivialType_DefaultCtor)
  {
    Vector<int> v;
    UNIT_TEST_REQUIRE(v.IsEmpty());
    UNIT_TEST_REQUIRE(v.Size() == 0);
    UNIT_TEST_REQUIRE(v.Capacity() == 0);
    UNIT_TEST_REQUIRE(v.Allocator() == Core::globalAllocator);
  }
  UNIT_TEST(Vector_TrivialType_CtorWithCustomAllocator)
  {
    NullAllocator alloc;
    Vector<int> v(&alloc);
    UNIT_TEST_REQUIRE(v.Allocator() == &alloc);
  }
}
