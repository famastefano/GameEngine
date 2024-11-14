#include <Core/Container/String.h>
#include <Core/Container/Vector.h>
#include <UnitTest/UnitTest.h>

UNIT_TEST_SUITE(StringView)
{
  using Core::StringView;
  using Core::Vector;

  UNIT_TEST(StringView_DefaultCtor)
  {
    StringView<char> WCS;
    UNIT_TEST_REQUIRE(WCS.IsEmpty());
    UNIT_TEST_REQUIRE(WCS.begin() == WCS.end());

    StringView<wchar_t> wcs;
    UNIT_TEST_REQUIRE(wcs.IsEmpty());
    UNIT_TEST_REQUIRE(wcs.begin() == wcs.end());
  }

  UNIT_TEST(StringView_FromStringLiteral)
  {
    StringView<char> WCS("01234");
    UNIT_TEST_REQUIRE(WCS.Size() == 5);
    {
      i32 i = 0;
      for (char c : WCS)
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

  UNIT_TEST(StringView_FrontReturnsTheFirstCharacter)
  {
    StringView<char> WCS("0123");
    UNIT_TEST_REQUIRE(WCS.Front() == '0');

    StringView<wchar_t> wcs(L"0123");
    UNIT_TEST_REQUIRE(wcs.Front() == L'0');
  }

  UNIT_TEST(StringView_BackReturnsTheLastCharacter)
  {
    StringView<char> WCS("0123");
    UNIT_TEST_REQUIRE(WCS.Back() == '3');

    StringView<wchar_t> wcs(L"0123");
    UNIT_TEST_REQUIRE(wcs.Back() == L'3');
  }

  UNIT_TEST(StringView_FirstAndBackReturnsSameCharIfStringViewHas1Char)
  {
    StringView<char> WCS("A");
    UNIT_TEST_REQUIRE(WCS.Front() == WCS.Back());
    UNIT_TEST_REQUIRE(WCS.Front() == 'A');

    StringView<wchar_t> wcs(L"A");
    UNIT_TEST_REQUIRE(wcs.Front() == wcs.Back());
    UNIT_TEST_REQUIRE(wcs.Front() == L'A');
  }

  UNIT_TEST(StringView_RemovePrefixWithEmptyViewReturnsAnotherEmptyView)
  {
    StringView<char> WCS;
    auto             otherCS = WCS.RemovePrefix(0);
    UNIT_TEST_REQUIRE(otherCS.IsEmpty());

    StringView<char> wcs;
    auto             otherWCS = wcs.RemovePrefix(0);
    UNIT_TEST_REQUIRE(otherWCS.IsEmpty());
  }

  UNIT_TEST(StringView_RemoveSuffixWithEmptyViewReturnsAnotherEmptyView)
  {
    StringView<char> WCS;
    auto             otherCS = WCS.RemoveSuffix(0);
    UNIT_TEST_REQUIRE(otherCS.IsEmpty());

    StringView<char> wcs;
    auto             otherWCS = wcs.RemoveSuffix(0);
    UNIT_TEST_REQUIRE(otherWCS.IsEmpty());
  }

  UNIT_TEST(StringView_RemovePrefixActuallyRemovesTheFirstNChars)
  {
    StringView<char> cs("12345");
    auto             otherCS = cs.RemovePrefix(3);
    UNIT_TEST_REQUIRE(otherCS.Size() == 2);
    UNIT_TEST_REQUIRE(otherCS[0] == '4');
    UNIT_TEST_REQUIRE(otherCS[1] == '5');

    StringView<wchar_t> wcs(L"12345");
    auto                otherWCS = wcs.RemovePrefix(3);
    UNIT_TEST_REQUIRE(otherWCS.Size() == 2);
    UNIT_TEST_REQUIRE(otherWCS[0] == '4');
    UNIT_TEST_REQUIRE(otherWCS[1] == '5');
  }

  UNIT_TEST(StringView_RemovePrefixCanRemoveTheEntireString)
  {
    StringView<char> cs("12345");
    auto             otherCS = cs.RemovePrefix(cs.Size());
    UNIT_TEST_REQUIRE(otherCS.IsEmpty());

    StringView<wchar_t> wcs(L"12345");
    auto                otherWCS = wcs.RemovePrefix(wcs.Size());
    UNIT_TEST_REQUIRE(otherWCS.IsEmpty());
  }

  UNIT_TEST(StringView_RemoveSuffixCanRemoveTheEntireString)
  {
    StringView<char> cs("12345");
    auto             otherCS = cs.RemoveSuffix(cs.Size());
    UNIT_TEST_REQUIRE(otherCS.IsEmpty());

    StringView<wchar_t> wcs(L"12345");
    auto                otherWCS = wcs.RemoveSuffix(wcs.Size());
    UNIT_TEST_REQUIRE(otherWCS.IsEmpty());
  }

  UNIT_TEST(StringView_RemoveSuffixWithData)
  {
    StringView<char> cs("12345");
    auto             otherCS = cs.RemoveSuffix(4);
    UNIT_TEST_REQUIRE(otherCS.Size() == 1);
    UNIT_TEST_REQUIRE(otherCS.Front() == cs.Front());

    StringView<wchar_t> wcs(L"12345");
    auto                otherWCS = wcs.RemoveSuffix(4);
    UNIT_TEST_REQUIRE(otherWCS.Size() == 1);
    UNIT_TEST_REQUIRE(otherWCS.Front() == wcs.Front());
  }

  UNIT_TEST(StringView_CopyOfEmptyNeverCopies)
  {
    Vector<char>     vcs(1, '\0');
    StringView<char> cs;
    i32 const        csCopied = cs.Copy(vcs.Data(), vcs.Size());
    UNIT_TEST_REQUIRE(csCopied == 0);

    Vector<wchar_t>     vwcs(1, L'\0');
    StringView<wchar_t> wcs;
    i32 const           wcsCopied = wcs.Copy(vwcs.Data(), vwcs.Size());
    UNIT_TEST_REQUIRE(wcsCopied == 0);
  }

  UNIT_TEST(StringView_CopyWorksIfNonEmpty)
  {
    Vector<char>     vcs(10, '\0');
    StringView<char> cs("1234");
    i32 const        csCopied = cs.Copy(vcs.Data(), cs.Size());
    UNIT_TEST_REQUIRE(csCopied == cs.Size());
    UNIT_TEST_REQUIRE(memcmp(vcs.Data(), cs.Data(), cs.Size()) == 0);

    Vector<wchar_t>     vwcs(10, '\0');
    StringView<wchar_t> wcs(L"1234");
    i32 const           wcsCopied = wcs.Copy(vwcs.Data(), wcs.Size());
    UNIT_TEST_REQUIRE(wcsCopied == wcs.Size());
    UNIT_TEST_REQUIRE(memcmp(vwcs.Data(), wcs.Data(), wcs.Size()) == 0);
  }

  UNIT_TEST(StringView_CopySubstr)
  {
    Vector<char>     vcs(2, '\0');
    StringView<char> cs("01234");
    i32 const        csCopied = cs.Copy(vcs.Data(), vcs.Size(), 1);
    UNIT_TEST_REQUIRE(csCopied == vcs.Size());
    UNIT_TEST_REQUIRE(memcmp(vcs.Data(), cs.Data() + 1, vcs.Size()) == 0);

    Vector<wchar_t>     vwcs(2, '\0');
    StringView<wchar_t> wcs(L"01234");
    i32 const           wcsCopied = wcs.Copy(vwcs.Data(), vwcs.Size(), 1);
    UNIT_TEST_REQUIRE(wcsCopied == vwcs.Size());
    UNIT_TEST_REQUIRE(memcmp(vwcs.Data(), wcs.Data() + 1, vwcs.Size()) == 0);
  }

  UNIT_TEST(StringView_SubStrOfEmptyReturnsAnotherEmpty)
  {
    StringView<char> cs;
    auto             otherCS = cs.SubStr(0, 0);
    UNIT_TEST_REQUIRE(otherCS.IsEmpty());

    StringView<wchar_t> wcs;
    auto                otherWCS = wcs.SubStr(0, 0);
    UNIT_TEST_REQUIRE(otherWCS.IsEmpty());
  }

  UNIT_TEST(StringView_SubStrWithZeroOffsetBehavesLikeRemoveSuffix)
  {
    StringView<char> cs("12345");
    auto             otherCS  = cs.SubStr(0, 3);
    auto             otherCSF = cs.RemoveSuffix(cs.Size() - 3);
    UNIT_TEST_REQUIRE(otherCS == otherCSF);

    StringView<wchar_t> wcs(L"12345");
    auto                otherWCS  = wcs.SubStr(0, 2);
    auto                otherWCSF = wcs.RemoveSuffix(wcs.Size() - 2);
    UNIT_TEST_REQUIRE(otherWCS == otherWCSF);
  }

  UNIT_TEST(StringView_SubStr)
  {
    StringView<char> cs("12345");
    auto             otherCS = cs.SubStr(1, 2);
    UNIT_TEST_REQUIRE(otherCS.Size() == 2);
    UNIT_TEST_REQUIRE(otherCS[0] == '2');
    UNIT_TEST_REQUIRE(otherCS[1] == '3');

    StringView<wchar_t> wcs(L"12345");
    auto                otherWCS = wcs.SubStr(2, 3);
    UNIT_TEST_REQUIRE(otherWCS.Size() == 3);
    UNIT_TEST_REQUIRE(otherWCS[0] == L'3');
    UNIT_TEST_REQUIRE(otherWCS[1] == L'4');
    UNIT_TEST_REQUIRE(otherWCS[2] == L'5');
  }

  UNIT_TEST(StringView_StartsWithNeverStartsWithEmpty)
  {
    StringView<char>    ecs;
    StringView<wchar_t> ewcs;

    StringView<char> cs0;
    UNIT_TEST_REQUIRE_FALSE(cs0.StartsWith(ecs));

    StringView<wchar_t> wcs0;
    UNIT_TEST_REQUIRE_FALSE(wcs0.StartsWith(ewcs));

    StringView<char> cs1("hello");
    UNIT_TEST_REQUIRE_FALSE(cs1.StartsWith(ecs));

    StringView<wchar_t> wcs1(L"hello");
    UNIT_TEST_REQUIRE_FALSE(wcs1.StartsWith(ewcs));
  }

  UNIT_TEST(StringView_StartsWithWorks)
  {
    StringView<char> cs("Hello World");
    UNIT_TEST_REQUIRE(cs.StartsWith("Hello"));
    UNIT_TEST_REQUIRE_FALSE(cs.StartsWith(" Hello"));
    UNIT_TEST_REQUIRE_FALSE(cs.StartsWith("AAAAAAAAAAAAAAAAAAAAAAAAAAA"));

    StringView<wchar_t> wcs(L"Hello World");
    UNIT_TEST_REQUIRE(wcs.StartsWith(L"Hello"));
    UNIT_TEST_REQUIRE_FALSE(wcs.StartsWith(L" Hello"));
    UNIT_TEST_REQUIRE_FALSE(wcs.StartsWith(L"BBBBBBBBBBBBBBBBBBBBBBBBB"));
  }

  UNIT_TEST(StringView_EndsWithNeverEndsWithEmpty)
  {
    StringView<char>    ecs;
    StringView<wchar_t> ewcs;

    StringView<char> cs0;
    UNIT_TEST_REQUIRE_FALSE(cs0.EndsWith(ecs));

    StringView<wchar_t> wcs0;
    UNIT_TEST_REQUIRE_FALSE(wcs0.EndsWith(ewcs));

    StringView<char> cs1("hello");
    UNIT_TEST_REQUIRE_FALSE(cs1.EndsWith(ecs));

    StringView<wchar_t> wcs1(L"hello");
    UNIT_TEST_REQUIRE_FALSE(wcs1.EndsWith(ewcs));
  }

  UNIT_TEST(StringView_EndsWithWorks)
  {
    StringView<char> cs("Hello World");
    UNIT_TEST_REQUIRE(cs.EndsWith(" World"));
    UNIT_TEST_REQUIRE_FALSE(cs.EndsWith("rld "));
    UNIT_TEST_REQUIRE_FALSE(cs.EndsWith("QQQQQQQQQQQQQQQQQQQQQQQQQQQQ"));

    StringView<wchar_t> wcs(L"Hello World");
    UNIT_TEST_REQUIRE(wcs.EndsWith(L"World"));
    UNIT_TEST_REQUIRE_FALSE(wcs.EndsWith(L"Word"));
    UNIT_TEST_REQUIRE_FALSE(wcs.EndsWith(L"EEEEEEEEEEEEEEEEEEEEEEEEEE"));
  }

  UNIT_TEST(StringView_EmptyNeverContainsAnything)
  {
    StringView<char> cs;
    UNIT_TEST_REQUIRE_FALSE(cs.Contains(""));
    UNIT_TEST_REQUIRE_FALSE(cs.Contains(StringView<char>()));
    UNIT_TEST_REQUIRE_FALSE(cs.Contains("Hello"));

    StringView<wchar_t> wcs;
    UNIT_TEST_REQUIRE_FALSE(wcs.Contains(L""));
    UNIT_TEST_REQUIRE_FALSE(wcs.Contains(StringView<wchar_t>()));
    UNIT_TEST_REQUIRE_FALSE(wcs.Contains(L"Hello"));
  }

  UNIT_TEST(StringView_ContainsWorks)
  {
    StringView<char> cs("Hello World");
    UNIT_TEST_REQUIRE_FALSE(cs.Contains("Hello World Hello World"));
    UNIT_TEST_REQUIRE(cs.Contains("lo Wo"));
    UNIT_TEST_REQUIRE(cs.Contains("Hello World"));
    for (char c : cs)
      UNIT_TEST_REQUIRE(cs.Contains(c));

    StringView<wchar_t> wcs(L"Hello World");
    UNIT_TEST_REQUIRE_FALSE(wcs.Contains(L"Hello World Hello World"));
    UNIT_TEST_REQUIRE(wcs.Contains(L"lo Wo"));
    UNIT_TEST_REQUIRE(wcs.Contains(L"Hello World"));
    for (wchar_t c : cs)
      UNIT_TEST_REQUIRE(wcs.Contains(c));
  }

  UNIT_TEST(StringView_FindOfEmptyNeverFindsAnything)
  {
    StringView<char> cs;
    UNIT_TEST_REQUIRE(cs.Find(cs) == cs.NotFound);
    UNIT_TEST_REQUIRE(cs.Find("") == cs.NotFound);
    UNIT_TEST_REQUIRE(cs.Find(StringView<char>{}) == cs.NotFound);
    UNIT_TEST_REQUIRE(cs.Find("Hello") == cs.NotFound);

    StringView<wchar_t> wcs;
    UNIT_TEST_REQUIRE(wcs.Find(wcs) == wcs.NotFound);
    UNIT_TEST_REQUIRE(wcs.Find(L"") == wcs.NotFound);
    UNIT_TEST_REQUIRE(wcs.Find(StringView<wchar_t>{}) == wcs.NotFound);
    UNIT_TEST_REQUIRE(wcs.Find(L"Hello") == wcs.NotFound);
  }

  UNIT_TEST(StringView_FindWorks)
  {
    StringView<char> cs("01234 56789");
    UNIT_TEST_REQUIRE(cs.Find("") == cs.NotFound);
    UNIT_TEST_REQUIRE(cs.Find(StringView<char>{}) == cs.NotFound);
    UNIT_TEST_REQUIRE(cs.Find("0123") == 0);
    UNIT_TEST_REQUIRE(cs.Find("34 5") == 3);
    UNIT_TEST_REQUIRE(cs.Find(cs) == 0);

    StringView<wchar_t> wcs(L"01234 56789");
    UNIT_TEST_REQUIRE(wcs.Find(L"") == wcs.NotFound);
    UNIT_TEST_REQUIRE(wcs.Find(StringView<wchar_t>{}) == wcs.NotFound);
    UNIT_TEST_REQUIRE(wcs.Find(L"Hello") == wcs.NotFound);
    UNIT_TEST_REQUIRE(wcs.Find(L"013") == wcs.NotFound);
    UNIT_TEST_REQUIRE(wcs.Find(L" 5") == 5);
    UNIT_TEST_REQUIRE(wcs.Find(L"9") == 10);
  }

  UNIT_TEST(StringView_FindLastOfNeverFindsAnythingIfEmpty)
  {
    StringView<char> cs;
    UNIT_TEST_REQUIRE(cs.FindLastOf(cs) == cs.NotFound);
    UNIT_TEST_REQUIRE(cs.FindLastOf("") == cs.NotFound);
    UNIT_TEST_REQUIRE(cs.FindLastOf("Hello") == cs.NotFound);

    StringView<wchar_t> wcs;
    UNIT_TEST_REQUIRE(wcs.FindLastOf(wcs) == wcs.NotFound);
    UNIT_TEST_REQUIRE(wcs.FindLastOf(L"") == wcs.NotFound);
    UNIT_TEST_REQUIRE(wcs.FindLastOf(L"Hello") == wcs.NotFound);
  }

  UNIT_TEST(StringView_FindLastOfWorks)
  {
    StringView<char> cs("Hello Hello Hello");
    UNIT_TEST_REQUIRE(cs.FindLastOf("o") == cs.Size() - 1);
    UNIT_TEST_REQUIRE(cs.FindLastOf("Hello") == strlen("Hello ") * 2);
    UNIT_TEST_REQUIRE(cs.FindLastOf(cs) == 0);
    UNIT_TEST_REQUIRE(cs.FindLastOf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA") == cs.NotFound);

    StringView<wchar_t> wcs(L"Hello Hello Hello");
    UNIT_TEST_REQUIRE(wcs.FindLastOf(L"o") == cs.Size() - 1);
    UNIT_TEST_REQUIRE(wcs.FindLastOf(L"Hello") == wcslen(L"Hello ") * 2);
    UNIT_TEST_REQUIRE(wcs.FindLastOf(wcs) == 0);
    UNIT_TEST_REQUIRE(wcs.FindLastOf(L"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA") == wcs.NotFound);
  }
}
