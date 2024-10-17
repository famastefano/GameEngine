#include <UnitTest/UnitTest.h>
#include <algorithm>
#include <format>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>

struct RunOptions
{
  std::string_view Suite{};
  bool             WantsParallelExecution{};
} Options{};

std::vector<UnitTest::Private::TestBase*> GetFilteredTests();

int main(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string_view arg = argv[i];
    if (arg == "--parallel")
      Options.WantsParallelExecution = true;
    else if (arg.starts_with("--test-suite="))
    {
      arg.remove_prefix(strlen("--test-suite="));
      Options.Suite = arg;
    }
  }

  auto const tests = GetFilteredTests();
  if (!Options.WantsParallelExecution)
  {
    for (auto* test : tests)
      test->Run();
  }
  else
  {
    std::vector<std::jthread> workers(std::thread::hardware_concurrency());
    std::atomic<int>          testId = 0;
    for (auto& worker : workers)
    {
      worker = std::jthread([&tests, &testId] {
        int const id = testId.fetch_add(1, std::memory_order_relaxed);
        if (id >= tests.size())
          return;
        tests[id]->Run();
      });
    }
  }

  int const passed = UnitTest::Private::GlobalPassedTestsCounter;
  int const failed = UnitTest::Private::GlobalFailedTestsCounter;
  std::cout << std::format("Total: {} Passed: {} Failed: {}\n", passed + failed, passed, failed);
}

std::vector<UnitTest::Private::TestBase*> GetFilteredTests()
{
  auto filtered = UnitTest::Private::TestBase::GetTests();
  if (Options.Suite.empty())
    return filtered;

  std::erase_if(filtered, [](UnitTest::Private::TestBase* test) {
    return std::string_view{test->SuiteName} != Options.Suite;
  });
  return filtered;
}
