#include <Core/Container/Span.h>
#include <UnitTest/UnitTest.h>

UNIT_TEST_SUITE(Span)
{
  using Core::Span;
  using Core::Vector;

  UNIT_TEST(Span_DynamicSize_DefaultCtorCreatesEmptySpan)
  {
    Span<i32> s;
    UNIT_TEST_REQUIRE(s.IsEmpty());
    UNIT_TEST_REQUIRE(s.begin() == s.end());
  }

  UNIT_TEST(Span_CompileSize_ZeroSizeIsEmpty)
  {
    Span<i32, 0> s;
    UNIT_TEST_REQUIRE(s.IsEmpty());
    UNIT_TEST_REQUIRE(s.begin() == s.end());
  }

  UNIT_TEST(Span_DynamicSize_CtorFromStaticArray)
  {
    i32       arr[5] = {0, 1, 2, 3, 4};
    Span<i32> s{arr};
    UNIT_TEST_REQUIRE(s.Size() == 5);
    i32 i = 0;
    for (i32 item : s)
      UNIT_TEST_REQUIRE(arr[i++] == item);
  }

  UNIT_TEST(Span_CompileSize_CtorFromStaticArray)
  {
    i32          arr[5] = {0, 1, 2, 3, 4};
    Span<i32, 5> s{arr};
    UNIT_TEST_REQUIRE(s.Size() == 5);
    i32 i = 0;
    for (i32 item : s)
      UNIT_TEST_REQUIRE(arr[i++] == item);
  }

  UNIT_TEST(Span_CompileSize_CtorFromStaticArray_LessSize)
  {
    i32          arr[5] = {0, 1, 2, 3, 4};
    Span<i32, 2> s{arr};
    UNIT_TEST_REQUIRE(s.Size() == 2);
    i32 i = 0;
    for (i32 item : s)
      UNIT_TEST_REQUIRE(arr[i++] == item);
  }

  UNIT_TEST(Span_DynamicSize_CtorFromIteratorAndCount)
  {
    i32       arr[5] = {0, 1, 2, 3, 4};
    Span<i32> s{arr, 5};
    UNIT_TEST_REQUIRE(s.Size() == 5);
    i32 i = 0;
    for (i32 item : s)
      UNIT_TEST_REQUIRE(arr[i++] == item);
  }

  UNIT_TEST(Span_DynamicSize_CtorFromIteratorAndCount_LessSize)
  {
    i32       arr[5] = {0, 1, 2, 3, 4};
    Span<i32> s{arr, 3};
    UNIT_TEST_REQUIRE(s.Size() == 3);
    i32 i = 0;
    for (i32 item : s)
      UNIT_TEST_REQUIRE(arr[i++] == item);
  }

  UNIT_TEST(Span_CompileSize_CtorFromIteratorAndCount)
  {
    i32          arr[5] = {0, 1, 2, 3, 4};
    Span<i32, 5> s{arr, 5};
    UNIT_TEST_REQUIRE(s.Size() == 5);
    i32 i = 0;
    for (i32 item : s)
      UNIT_TEST_REQUIRE(arr[i++] == item);
  }

  UNIT_TEST(Span_CompileSize_CtorFromIteratorAndCount_LessSize)
  {
    i32          arr[5] = {0, 1, 2, 3, 4};
    Span<i32, 3> s{arr, 3};
    UNIT_TEST_REQUIRE(s.Size() == 3);
    i32 i = 0;
    for (i32 item : s)
      UNIT_TEST_REQUIRE(arr[i++] == item);
  }

  UNIT_TEST(Span_DynamicSize_CtorFromIteratorPair)
  {
    i32       arr[5] = {0, 1, 2, 3, 4};
    Span<i32> s{arr, arr + 5};
    UNIT_TEST_REQUIRE(s.Size() == 5);
    i32 i = 0;
    for (i32 item : s)
      UNIT_TEST_REQUIRE(arr[i++] == item);
  }

  UNIT_TEST(Span_DynamicSize_CtorFromIteratorPair_LessSize)
  {
    i32       arr[5] = {0, 1, 2, 3, 4};
    Span<i32> s{arr + 1, arr + 3};
    UNIT_TEST_REQUIRE(s.Size() == 2);
    i32 i = 1;
    for (i32 item : s)
      UNIT_TEST_REQUIRE(arr[i++] == item);
  }

  UNIT_TEST(Span_CompileSize_CtorFromIteratorPair)
  {
    i32          arr[5] = {0, 1, 2, 3, 4};
    Span<i32, 5> s{arr, arr + 5};
    UNIT_TEST_REQUIRE(s.Size() == 5);
    i32 i = 0;
    for (i32 item : s)
      UNIT_TEST_REQUIRE(arr[i++] == item);
  }

  UNIT_TEST(Span_CompileSize_CtorFromIteratorPair_LessSize)
  {
    i32          arr[5] = {0, 1, 2, 3, 4};
    Span<i32, 3> s{arr + 2, arr + 5};
    UNIT_TEST_REQUIRE(s.Size() == 3);
    i32 i = 2;
    for (i32 item : s)
      UNIT_TEST_REQUIRE(arr[i++] == item);
  }

  UNIT_TEST(Span_DynamicSize_CtorFromEmptyVector)
  {
    Vector<i32> v;
    Span<i32>   s{v};
    UNIT_TEST_REQUIRE(s.IsEmpty());
    UNIT_TEST_REQUIRE(s.begin() == s.end());
  }

  UNIT_TEST(Span_CompileSize_CtorFromEmptyVector)
  {
    Vector<i32>  v;
    Span<i32, 0> s{v};
    UNIT_TEST_REQUIRE(s.IsEmpty());
    UNIT_TEST_REQUIRE(s.begin() == s.end());
  }

  UNIT_TEST(Span_DynamicSize_CtorFromNonEmptyVector)
  {
    Vector<i32> v{0, 1, 2};
    Span<i32>   s{v};
    UNIT_TEST_REQUIRE(s.Size() == v.Size());
    for (i32 i = 0; i < s.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i] == v[i]);
  }

  UNIT_TEST(Span_CompileSize_CtorFromNonEmptyVector)
  {
    Vector<i32>  v{0, 1, 2};
    Span<i32, 3> s{v};
    UNIT_TEST_REQUIRE(s.Size() == v.Size());
    for (i32 i = 0; i < s.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i] == v[i]);
  }

  UNIT_TEST(Span_CompileSize_CtorFromNonEmptyVector_LessSize)
  {
    Vector<i32>  v{0, 1, 2};
    Span<i32, 2> s{v};
    UNIT_TEST_REQUIRE(s.Size() == 2);
    for (i32 i = 0; i < s.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i] == v[i]);
  }

  UNIT_TEST(Span_DynamicSize_FirstOfEmptyReturnsAnotherEmpty)
  {
    Span<i32> s;
    auto      other = s.First(0);
    UNIT_TEST_REQUIRE(s.IsEmpty());
    UNIT_TEST_REQUIRE(other.IsEmpty());
  }

  UNIT_TEST(Span_CompileSize_FirstOfEmptyReturnsAnotherEmpty)
  {
    Span<i32, 0> s;
    auto         other = s.First<0>();
    UNIT_TEST_REQUIRE(s.IsEmpty());
    UNIT_TEST_REQUIRE(other.IsEmpty());
  }

  UNIT_TEST(Span_DynamicSize_FirstWithSameSizeReturnsACopy)
  {
    Vector<i32>     v{1, 2, 3};
    Span<i32 const> s{v};
    auto            other = s.First(3);
    UNIT_TEST_REQUIRE(s.Size() == other.Size());
    for (i32 i = 0; i < other.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i] == other[i]);
  }

  UNIT_TEST(Span_CompileSize_FirstWithSameSizeReturnsACopy)
  {
    Vector<i32>        v{1, 2, 3};
    Span<i32 const, 3> s{v};
    auto               other = s.First<3>();
    UNIT_TEST_REQUIRE(s.Size() == other.Size());
    for (i32 i = 0; i < other.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i] == other[i]);
  }

  UNIT_TEST(Span_DynamicSize_FirstWithLessSize)
  {
    Vector<i32> v{1, 2, 3};
    Span<i32>   s{v};
    auto        other = s.First(2);
    for (i32 i = 0; i < other.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i] == other[i]);
  }

  UNIT_TEST(Span_CompileSize_FirstWithLessSize)
  {
    Vector<i32>  v{1, 2, 3};
    Span<i32, 3> s{v};
    auto         other = s.First<2>();
    for (i32 i = 0; i < other.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i] == other[i]);
  }

  UNIT_TEST(Span_DynamicSize_LastOfEmptyReturnsAnotherEmpty)
  {
    Span<i32> s;
    auto      other = s.Last(0);
    UNIT_TEST_REQUIRE(s.IsEmpty());
    UNIT_TEST_REQUIRE(other.IsEmpty());
  }

  UNIT_TEST(Span_CompileSize_LastOfEmptyReturnsAnotherEmpty)
  {
    Span<i32, 0> s;
    auto         other = s.Last<0>();
    UNIT_TEST_REQUIRE(s.IsEmpty());
    UNIT_TEST_REQUIRE(other.IsEmpty());
  }

  UNIT_TEST(Span_DynamicSize_LastWithSameSizeReturnsACopy)
  {
    Vector<i32>     v{1, 2, 3};
    Span<i32 const> s{v};
    auto            other = s.Last(3);
    UNIT_TEST_REQUIRE(s.Size() == other.Size());
    for (i32 i = 0; i < other.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i] == other[i]);
  }

  UNIT_TEST(Span_CompileSize_LastWithSameSizeReturnsACopy)
  {
    Vector<i32>        v{1, 2, 3};
    Span<i32 const, 3> s{v};
    auto               other = s.Last<3>();
    UNIT_TEST_REQUIRE(s.Size() == other.Size());
    for (i32 i = 0; i < other.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i] == other[i]);
  }

  UNIT_TEST(Span_DynamicSize_LastWithLessSize)
  {
    Vector<i32> v{1, 2, 3};
    Span<i32>   s{v};
    auto        other = s.Last(2);
    for (i32 i = 0; i < other.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i + 1] == other[i]);
  }

  UNIT_TEST(Span_CompileSize_LastWithLessSize)
  {
    Vector<i32>  v{1, 2, 3};
    Span<i32, 3> s{v};
    auto         other = s.Last<2>();
    for (i32 i = 0; i < other.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i + 1] == other[i]);
  }

  UNIT_TEST(Span_DynamicSize_SubSpanWithSameSizeReturnsACopy)
  {
    Vector<i32>     v{1, 2, 3};
    Span<i32 const> s{v};
    auto            other = s.SubSpan(0, 3);
    UNIT_TEST_REQUIRE(s.Size() == other.Size());
    for (i32 i = 0; i < other.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i] == other[i]);
  }

  UNIT_TEST(Span_CompileSize_SubSpanWithSameSizeReturnsACopy)
  {
    Vector<i32>        v{1, 2, 3};
    Span<i32 const, 3> s{v};
    auto               other = s.SubSpan<0, 3>();
    UNIT_TEST_REQUIRE(s.Size() == other.Size());
    for (i32 i = 0; i < other.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i] == other[i]);
  }

  UNIT_TEST(Span_DynamicSize_SubSpanWithLessSize)
  {
    Vector<i32> v{1, 2, 3};
    Span<i32>   s{v};
    auto        other = s.SubSpan(1, 2);
    for (i32 i = 0; i < other.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i + 1] == other[i]);
  }

  UNIT_TEST(Span_CompileSize_SubSpanWithLessSize)
  {
    Vector<i32>  v{1, 2, 3};
    Span<i32, 3> s{v};
    auto         other = s.SubSpan<1, 2>();
    for (i32 i = 0; i < other.Size(); ++i)
      UNIT_TEST_REQUIRE(s[i + 1] == other[i]);
  }
}
