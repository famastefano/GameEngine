#include <UnitTest/UnitTest.h>
#include <algorithm>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>

static std::mutex GlobalOutputMutex;

namespace UnitTest::Private
{
std::atomic<int> GlobalPassedTestsCounter = 0;
std::atomic<int> GlobalFailedTestsCounter = 0;

TestBase::TestBase(char const* SuiteName, char const* TestName)
    : SuiteName_(SuiteName)
    , TestName_(TestName)
{
  GetTests().EmplaceBack(this);
}
void TestBase::Run(TestOptions const& options)
{
  std::string msg  = "TEST ";
  msg             += SuiteName_;
  msg             += '.';
  msg             += TestName_;

  ExecuteTest();

  std::string endMsg = "RESULT: ";
  if (FailReason_)
  {
    endMsg += "FAILED - ";
    endMsg += FailReason_;
    UnitTest::Private::GlobalFailedTestsCounter.fetch_add(1, std::memory_order_relaxed);
  }
  else
  {
    endMsg += "OK";
    UnitTest::Private::GlobalPassedTestsCounter.fetch_add(1, std::memory_order_relaxed);
  }

  if (options.OutputOnlyIfFailed && FailReason_ || !options.OutputOnlyIfFailed)
  {
    std::scoped_lock lck{GlobalOutputMutex};
    std::cout << msg << '\n'
              << endMsg << '\n'
              << '\n';
  }
}
Core::Vector<TestBase*>& TestBase::GetTests()
{
  static Core::Vector<TestBase*> tests;
  return tests;
}
void TestBase::MarkAsFailed(char const* reason)
{
  FailReason_ = reason;
}
} // namespace UnitTest::Private
