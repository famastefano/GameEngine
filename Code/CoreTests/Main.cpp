#include <Core/Container/String.h>
#include <UnitTest/UnitTest.h>
#include <algorithm>
#include <format>
#include <iostream>
#include <thread>

struct RunOptions
{
  Core::StringView<char> Suite{};
  bool                   WantsParallelExecution{};
  bool                   OutputOnlyIfFailed{};
} Options{};

Core::Vector<UnitTest::Private::TestBase*> GetFilteredTests();
UnitTest::TestOptions                      MakeTestOptions();

int main(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    Core::StringView<char> arg = argv[i];
    if (arg == "--parallel")
    {
      Options.WantsParallelExecution = true;
    }
    else if (arg.StartsWith("--test-suite="))
    {
      arg = arg.RemovePrefix(i32(strlen("--test-suite=")));
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
  if (Options.Suite.IsEmpty())
    return filtered;

  filtered.EraseIf([](UnitTest::Private::TestBase* test) {
    return Core::StringView<char>(test->SuiteName_) != Options.Suite;
  });
  return filtered;
}

UnitTest::TestOptions MakeTestOptions()
{
  UnitTest::TestOptions opt{};
  opt.OutputOnlyIfFailed = Options.OutputOnlyIfFailed;
  return opt;
}
