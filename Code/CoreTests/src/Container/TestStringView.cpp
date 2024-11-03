#include <Core/Container/String.h>
#include <UnitTest/UnitTest.h>

UNIT_TEST_SUITE(StringView)
{
  using Core::StringView;
  UNIT_TEST(StringView_DefaultCtor)
  {
    StringView<char> cs;
    UNIT_TEST_REQUIRE(cs.IsEmpty());
    UNIT_TEST_REQUIRE(cs.begin() == cs.end());

    StringView<wchar_t> wcs;
    UNIT_TEST_REQUIRE(wcs.IsEmpty());
    UNIT_TEST_REQUIRE(wcs.begin() == wcs.end());
  }

  UNIT_TEST(StringView_FromStringLiteral)
  {
    StringView<char> cs("01234");
    UNIT_TEST_REQUIRE(cs.Size() == 5);
    {
      i32 i = 0;
      for (char c : cs)
        UNIT_TEST_REQUIRE(c == "01234"[i++]);
    }

    StringView<wchar_t> wcs(L"01234");
    UNIT_TEST_REQUIRE(wcs.Size() == 5);
    {
      i32 i = 0;
      for (wchar_t c : wcs)
        UNIT_TEST_REQUIRE(c == L"01234"[i++]);
    }
  }
}
