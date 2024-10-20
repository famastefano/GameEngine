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
    Vector<int>   v(&alloc);
    UNIT_TEST_REQUIRE(v.Allocator() == &alloc);
  }
  UNIT_TEST(Vector_TrivialType_CtorWithInitialSize)
  {
    Vector<int> v(10);
    UNIT_TEST_REQUIRE_FALSE(v.IsEmpty());
    UNIT_TEST_REQUIRE(v.Size() == 10);
    UNIT_TEST_REQUIRE(v.Capacity() == 10);
    UNIT_TEST_REQUIRE(v.Allocator() == Core::globalAllocator);
  }
  UNIT_TEST(Vector_TrivialType_CtorWithInitialSizeAndValue)
  {
    Vector<int> v(512, 0xBE'EF);
    UNIT_TEST_REQUIRE(v.Size() == 512);
    for (int const& i : v)
      UNIT_TEST_REQUIRE(i == 0xBE'EF);
  }
  UNIT_TEST(Vector_TrivialType_CtorWithInitList)
  {
    Vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    UNIT_TEST_REQUIRE(v.Size() == 10);
    for (i32 i = 0; i < v.Size(); ++i)
      UNIT_TEST_REQUIRE(v[i] == i);
  }
  UNIT_TEST(Vector_TrivialType_CtorFromIterators)
  {
    int         arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    Vector<int> v(std::begin(arr), std::end(arr));
    UNIT_TEST_REQUIRE(v.Size() == 10);
    for (i32 i = 0; i < v.Size(); ++i)
      UNIT_TEST_REQUIRE(v[i] == arr[i]);
  }
}

/*
template <typename U = T>
Vector(i32 const initialSize, U const& initialValue, IAllocator* allocator = globalAllocator);

template <std::input_iterator Iterator>
Vector(Iterator begin, Iterator end, IAllocator* allocator = globalAllocator);

Vector(Vector const& other);
Vector(Vector const& other, IAllocator* allocator);

Vector(Vector&& other);

~Vector();
*/
