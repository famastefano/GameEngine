#include <UnitTest/UnitTest.h>
#include <algorithm>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>

static std::mutex GlobalOutputMutex;

namespace UnitTest::Private
{
std::atomic<int>       GlobalPassedTestsCounter = 0;
std::atomic<int>       GlobalFailedTestsCounter = 0;

TestBase::TestBase(char const* SuiteName, char const* TestName)
    : SuiteName(SuiteName)
    , TestName(TestName)
{
  GetTests().emplace_back(this);
}
void TestBase::Run()
{
  std::string msg  = "TEST ";
  msg             += SuiteName;
  msg             += '.';
  msg             += TestName;

  ExecuteTest();

  std::string endMsg = "RESULT: ";
  if (HasFailed)
  {
    endMsg += "FAILED";
    UnitTest::Private::GlobalFailedTestsCounter.fetch_add(1, std::memory_order_relaxed);
  }
  else
  {
    endMsg += "OK";
    UnitTest::Private::GlobalPassedTestsCounter.fetch_add(1, std::memory_order_relaxed);
  }

  std::scoped_lock lck{GlobalOutputMutex};
  std::cout << msg << '\n'
            << endMsg << '\n'
            << '\n';
}
std::vector<TestBase*>& TestBase::GetTests()
{
  static std::vector<TestBase*> tests;
  return tests;
}
void TestBase::MarkAsFailed()
{
  HasFailed = true;
}
} // namespace UnitTest::Private
