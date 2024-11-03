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
  bool             OutputOnlyIfFailed{};
} Options{};

Core::Vector<UnitTest::Private::TestBase*> GetFilteredTests();
UnitTest::TestOptions                      MakeTestOptions();

int main(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string_view arg = argv[i];
    if (arg == "--parallel")
    {
      Options.WantsParallelExecution = true;
    }
    else if (arg.starts_with("--test-suite="))
    {
      arg.remove_prefix(strlen("--test-suite="));
      Options.Suite = arg;
    }
    else if (arg == "--output-only-failed")
    {
      Options.OutputOnlyIfFailed = true;
    }
  }

  auto const testOptions = MakeTestOptions();
  auto const tests       = GetFilteredTests();
  if (!Options.WantsParallelExecution)
  {
    for (auto* test : tests)
      test->Run(testOptions);
  }
  else
  {
    Core::Vector<std::jthread> workers(std::thread::hardware_concurrency());
    std::atomic<int>           testId = 0;
    for (auto& worker : workers)
    {
      worker = std::jthread([&testOptions, &tests, &testId] {
        int const id = testId.fetch_add(1, std::memory_order_relaxed);
        if (id >= tests.Size())
          return;
        tests[id]->Run(testOptions);
      });
    }
  }

  int const passed = UnitTest::Private::GlobalPassedTestsCounter;
  int const failed = UnitTest::Private::GlobalFailedTestsCounter;
  std::cout << std::format("Total: {} Passed: {} Failed: {}\n", passed + failed, passed, failed);
}

Core::Vector<UnitTest::Private::TestBase*> GetFilteredTests()
{
  auto filtered = UnitTest::Private::TestBase::GetTests();
  if (Options.Suite.empty())
    return filtered;

  while (true)
  {
    auto* item = filtered.Find([](UnitTest::Private::TestBase* test) {
      return std::string_view{test->SuiteName_} != Options.Suite;
    });
    if (!item)
      return filtered;
    filtered.Erase(item);
  }
}

UnitTest::TestOptions MakeTestOptions()
{
  UnitTest::TestOptions opt{};
  opt.OutputOnlyIfFailed = Options.OutputOnlyIfFailed;
  return opt;
}
