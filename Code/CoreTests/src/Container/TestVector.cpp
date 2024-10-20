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
  UNIT_TEST(Vector_TrivialType_CtorWithInitialSize)
  {
    Vector<int> v(10);
    UNIT_TEST_REQUIRE_FALSE(v.IsEmpty());
    UNIT_TEST_REQUIRE(v.Size() == 10);
    UNIT_TEST_REQUIRE(v.Capacity() == 10);
    UNIT_TEST_REQUIRE(v.Allocator() == Core::globalAllocator);
  }
}


  /*Vector(IAllocator* allocator = globalAllocator);
  Vector(i32 const initialSize, IAllocator* allocator = globalAllocator);

  template <typename U = T>
  Vector(std::initializer_list<U> init, IAllocator* allocator = globalAllocator);

  template <typename U = T>
  Vector(i32 const initialSize, U const& initialValue, IAllocator* allocator = globalAllocator);

  template <std::input_iterator Iterator>
  Vector(Iterator begin, Iterator end, IAllocator* allocator = globalAllocator);

  Vector(Vector const& other);
  Vector(Vector const& other, IAllocator* allocator);

  Vector(Vector&& other);

  ~Vector();*/