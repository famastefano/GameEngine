#include <Core/Container/Vector.h>
#include <UnitTest/UnitTest.h>

UNIT_TEST_SUITE(Container)
{
  using Core::Vector;

  struct NullAllocator : Core::IAllocator
  {
    void* Alloc(i64 const size, i32 const alignment)
    {
      return nullptr;
    }
    void* Realloc(void* p, i64 const size, i32 const alignment)
    {
      return nullptr;
    }
    void Free(void* p, i32 const alignment)
    {
    }
    bool IsMovable()
    {
      return true;
    }
    bool IsCopyable()
    {
      return true;
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
  UNIT_TEST(Vector_TrivialType_CopyCtorFromEmpty)
  {
    Vector<int> v0;
    Vector<int> v1(v0);
    UNIT_TEST_REQUIRE(v0.Size() == v1.Size());
    UNIT_TEST_REQUIRE(v0.Capacity() == v1.Capacity());
    UNIT_TEST_REQUIRE(v0.Allocator() == v1.Allocator());
  }
  UNIT_TEST(Vector_TrivialType_CopyCtorFromEmptyWithCustomAllocator)
  {
    NullAllocator alloc;
    Vector<int>   v0(&alloc);
    Vector<int>   v1(v0);
    UNIT_TEST_REQUIRE(v0.Size() == v1.Size());
    UNIT_TEST_REQUIRE(v0.Capacity() == v1.Capacity());
    UNIT_TEST_REQUIRE(v0.Allocator() == v1.Allocator());
    UNIT_TEST_REQUIRE(v1.Allocator() == &alloc);
  }
  UNIT_TEST(Vector_TrivialType_CopyCtorFromEmptyWithCustomAllocatorAsParam)
  {
    NullAllocator alloc;
    Vector<int>   v0(&alloc);
    Vector<int>   v1(v0, Core::globalAllocator);
    UNIT_TEST_REQUIRE(v0.Size() == v1.Size());
    UNIT_TEST_REQUIRE(v0.Capacity() == v1.Capacity());
    UNIT_TEST_REQUIRE_FALSE(v0.Allocator() == v1.Allocator());
    UNIT_TEST_REQUIRE(v1.Allocator() == Core::globalAllocator);
  }
  UNIT_TEST(Vector_TrivialType_MoveCtorFromEmpty)
  {
    Vector<int> v0;
    i32 const   v0_sz  = v0.Size();
    i32 const   v0_cap = v0.Capacity();

    Vector<int> v1(std::move(v0));
    UNIT_TEST_REQUIRE(v1.Size() == v0_sz);
    UNIT_TEST_REQUIRE(v1.Capacity() == v0_cap);
    UNIT_TEST_REQUIRE(v1.Allocator() == Core::globalAllocator);

    UNIT_TEST_REQUIRE(v0.Allocator() == v1.Allocator());
    UNIT_TEST_REQUIRE(v0.IsEmpty());
    UNIT_TEST_REQUIRE(v0.Capacity() == 0);
  }
  UNIT_TEST(Vector_TrivialType_MoveCtorFromInitialized)
  {
    Vector<int> v0{0, 1, 2, 3};
    i32 const   v0_sz  = v0.Size();
    i32 const   v0_cap = v0.Capacity();

    Vector<int> v1(std::move(v0));
    UNIT_TEST_REQUIRE(v1.Size() == v0_sz);
    UNIT_TEST_REQUIRE(v1.Capacity() == v0_cap);
    UNIT_TEST_REQUIRE(v1.Allocator() == Core::globalAllocator);

    UNIT_TEST_REQUIRE(v0.Allocator() == v1.Allocator());
    UNIT_TEST_REQUIRE(v0.IsEmpty());
    UNIT_TEST_REQUIRE(v0.Capacity() == 0);
  }
  UNIT_TEST(Vector_TrivialType_CopyAssignFromEmpty)
  {
    Vector<int> v0;
    Vector<int> v1 = v0;
    UNIT_TEST_REQUIRE(v0.Size() == v1.Size());
    UNIT_TEST_REQUIRE(v0.Capacity() == v1.Capacity());
    UNIT_TEST_REQUIRE(v0.Allocator() == v1.Allocator());
  }
  UNIT_TEST(Vector_TrivialType_CopyAssignFromInitialized)
  {
    Vector<int> v0{0, 1, 2, 3};
    Vector<int> v1 = v0;
    UNIT_TEST_REQUIRE(v0.Size() == v1.Size());
    UNIT_TEST_REQUIRE(v0.Capacity() == v1.Capacity());
    UNIT_TEST_REQUIRE(v0.Allocator() == v1.Allocator());
    UNIT_TEST_REQUIRE(memcmp(v0.Data(), v1.Data(), v0.AllocSize()) == 0);
  }
  UNIT_TEST(Vector_TrivialType_MoveAssignFromEmpty)
  {
    Vector<int> v0;
    i32 const   v0_sz  = v0.Size();
    i32 const   v0_cap = v0.Capacity();

    Vector<int> v1 = std::move(v0);
    UNIT_TEST_REQUIRE(v1.Size() == v0_sz);
    UNIT_TEST_REQUIRE(v1.Capacity() == v0_cap);
    UNIT_TEST_REQUIRE(v1.Allocator() == Core::globalAllocator);

    UNIT_TEST_REQUIRE(v0.Allocator() == v1.Allocator());
    UNIT_TEST_REQUIRE(v0.IsEmpty());
    UNIT_TEST_REQUIRE(v0.Capacity() == 0);
  }
  UNIT_TEST(Vector_TrivialType_MoveAssignFromInitialized)
  {
    Vector<int> v0{0, 1, 2, 3};
    i32 const   v0_sz  = v0.Size();
    i32 const   v0_cap = v0.Capacity();

    Vector<int> v1 = std::move(v0);
    UNIT_TEST_REQUIRE(v1.Size() == v0_sz);
    UNIT_TEST_REQUIRE(v1.Capacity() == v0_cap);
    UNIT_TEST_REQUIRE(v1.Allocator() == Core::globalAllocator);

    UNIT_TEST_REQUIRE(v0.Allocator() == v1.Allocator());
    UNIT_TEST_REQUIRE(v0.IsEmpty());
    UNIT_TEST_REQUIRE(v0.Capacity() == 0);
  }
  UNIT_TEST(Vector_TrivialType_AssignWithValue)
  {
    Vector<int> v;
    v.Assign(512, 0xDE'AD);
    UNIT_TEST_REQUIRE(v.Size() == 512);
    UNIT_TEST_REQUIRE(v.Capacity() == 512);
    for (auto const& i : v)
      UNIT_TEST_REQUIRE(i == 0xDE'AD);
  }
  UNIT_TEST(Vector_TrivialType_AssignWithIterators)
  {
    int         arr[] = {-1, -2, -3, -4};
    Vector<int> v;
    v.Assign(std::begin(arr), std::end(arr));
    UNIT_TEST_REQUIRE(v.Size() == 4);
    UNIT_TEST_REQUIRE(v.Capacity() == 4);
    for (i32 i = 0; i < v.Size(); ++i)
      UNIT_TEST_REQUIRE(v[i] == arr[i]);
  }
  UNIT_TEST(Vector_TrivialType_NonEmptyVectorFrontEqualsDataAndBegin)
  {
    Vector<int> v{0, 1, 2};
    UNIT_TEST_REQUIRE(v.Front() == 0);
    UNIT_TEST_REQUIRE(*v.Data() == v.Front());
    UNIT_TEST_REQUIRE(v.Data() == v.begin());
  }
  UNIT_TEST(Vector_TrivialType_NonEmptyVectorBack)
  {
    Vector<int> v{0, 1, 2};
    UNIT_TEST_REQUIRE(v.Back() == 2);
    UNIT_TEST_REQUIRE(v.Back() == *(v.Data() + v.Size() - 1));
  }
  UNIT_TEST(Vector_TrivialType_EmptyVectorHasBeginEqualsEnd)
  {
    Vector<int> v;
    UNIT_TEST_REQUIRE(v.begin() == v.end());
  }
  UNIT_TEST(Vector_TrivialType_EmptyVectorHasNullptrData)
  {
    Vector<int> v;
    UNIT_TEST_REQUIRE(v.Data() == nullptr);
  }
  UNIT_TEST(Vector_TrivialType_ReserveChangesTheCapacityToExactlyThatValue)
  {
    Vector<int> v;
    v.Reserve(97'364);
    UNIT_TEST_REQUIRE(v.IsEmpty());
    UNIT_TEST_REQUIRE(v.Capacity() == 97'364);
  }
  UNIT_TEST(Vector_TrivialType_ReserveIsNOPIfCapacityAlreadyGreater)
  {
    Vector<int> v(10);
    v.Reserve(5);
    UNIT_TEST_REQUIRE(v.Capacity() == 10);
  }
  UNIT_TEST(Vector_TrivialType_ResizeOnEmptyVectorBehavesLikeCtorWithSize)
  {
    Vector<int> v0(5'431);
    Vector<int> v1;
    v1.Resize(v0.Size());
    UNIT_TEST_REQUIRE(v1.Size() == 5'431);
    UNIT_TEST_REQUIRE(v1.Capacity() == 5'431);
  }
  UNIT_TEST(Vector_TrivialType_ResizeWithCustomValue)
  {
    Vector<int> v;
    v.Resize(10, 52);
    UNIT_TEST_REQUIRE(v.Size() == 10);
    for (auto const& value : v)
      UNIT_TEST_REQUIRE(value == 52);
  }
  UNIT_TEST(Vector_TrivialType_ResizeDestroysElementsIfNewSizeIsSmaller)
  {
    Vector<int> v(10, 42);
    v.Resize(2, 99);
    UNIT_TEST_REQUIRE(v.Size() == 2);
    for (auto const& value : v)
      UNIT_TEST_REQUIRE(value == 42);
  }
  UNIT_TEST(Vector_TrivialType_ResizeAppendsElementsIfNewSizeIsBigger)
  {
    Vector<int> v(10, 42);
    v.Resize(20, 99);
    UNIT_TEST_REQUIRE(v.Size() == 20);
    for (i32 i = 0; i < 10; ++i)
      UNIT_TEST_REQUIRE(v[i] == 42);
    for (i32 i = 10; i < 20; ++i)
      UNIT_TEST_REQUIRE(v[i] == 99);
  }
  UNIT_TEST(Vector_TrivialType_ClearOfEmptyDoesNothing)
  {
    Vector<int> v;
    v.Clear();
    UNIT_TEST_REQUIRE(v.IsEmpty());
    UNIT_TEST_REQUIRE(v.Capacity() == 0);
  }
  UNIT_TEST(Vector_TrivialType_ClearOfReservedLeavesCapacityUnchanged)
  {
    Vector<int> v;
    v.Reserve(20);
    v.Clear();
    UNIT_TEST_REQUIRE(v.IsEmpty());
    UNIT_TEST_REQUIRE(v.Capacity() == 20);
  }
  UNIT_TEST(Vector_TrivialType_ClearOfInitializedLeavesCapacityUnchanged)
  {
    Vector<int> v = {1, 2, 3, 4, 5};
    v.Clear();
    UNIT_TEST_REQUIRE(v.IsEmpty());
    UNIT_TEST_REQUIRE(v.Capacity() == 5);
  }
}
