#pragma once

#include <atomic>
#include <functional>
#include <vector>

namespace UnitTest::Private
{
class TestBase
{
public:
  char const* SuiteName;
  char const* TestName;

  TestBase(char const* SuiteName, char const* TestName);
  void Run();

  static std::vector<TestBase*>& GetTests();

protected:
  virtual void ExecuteTest() = 0;

  void MarkAsFailed();

private:
  bool HasFailed = false;
};

extern std::vector<TestBase*> GlobalTestCollection;
extern std::atomic<int>       GlobalPassedTestsCounter;
extern std::atomic<int>       GlobalFailedTestsCounter;
} // namespace UnitTest::Private

#define UNIT_TEST_SUITE(Name)                        \
  namespace UnitTest::##Name::Private                \
  {                                                  \
    constexpr char const* InternalSuiteName = #Name; \
  }                                                  \
  namespace UnitTest::##Name

#define UNIT_TEST(Name)                                                  \
  class AutoRegisteringTest_##Name final : UnitTest::Private::TestBase   \
  {                                                                      \
  public:                                                                \
    AutoRegisteringTest_##Name()                                         \
        : UnitTest::Private::TestBase(Private::InternalSuiteName, #Name) \
    {                                                                    \
    }                                                                    \
                                                                         \
  protected:                                                             \
    virtual void ExecuteTest() override;                                 \
  };                                                                     \
  static AutoRegisteringTest_##Name _unit_test_autoreg_##Name;           \
  void                              AutoRegisteringTest_##Name::ExecuteTest()

#define UNIT_TEST_REQUIRE(Expr) \
  {                             \
    const bool result = (Expr); \
    if (!result)                \
    {                           \
      MarkAsFailed();           \
      return;                   \
    }                           \
  }
#define UNIT_TEST_REQUIRE_FALSE(expr) UNIT_TEST_REQUIRE(!(expr))
#define UNIT_TEST_FAIL() \
  {                      \
    MarkAsFailed();      \
    return;              \
  }
#define UNIT_TEST_PASS() \
  {                      \
    return;              \
  }
