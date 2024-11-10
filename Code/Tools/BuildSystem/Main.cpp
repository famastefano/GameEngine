#include "Helpers.h"
#include "Module/Module.h"
#include "Module/Parser.h"
#include "Module/Resolver.h"
#include "Module/Validator.h"

#include <Core/Container/FlatMap.h>
#include <Core/Definitions.h>
#include <Windows.h>
#include <filesystem>
#include <format>
#include <iostream>
#include <rapidjson/document.h>

Core::FlatMap<u64, Module> FindAllModules(Core::StringView<char> path);

int main(int argc, char** argv)
{
  // Find all module definitions
  //
  // While no issues found:
  // - Validate modules, remove invalid ones
  //     1. Missing required fields
  //     2. Missing dependencies
  // - Check for cyclical dependencies, remove leafs creating cycles
  // End
  //
  // Resolve inherits
  // Generate build

  if (argc != 2)
    return -1;

  Core::Vector<Module> modules;

  {
    auto foundModules = FindAllModules(argv[1]);
    modules           = ResolveModules(foundModules);
  }

  std::cout << "Final modules:\n";
  for (auto const& m : modules)
    std::cout << m.Name << "\n";
}

Core::FlatMap<u64, Module> FindAllModules(Core::StringView<char> path)
{
  Core::FlatMap<u64, Module> modules;

  std::error_code ec;
  for (auto const& dir : std::filesystem::recursive_directory_iterator(path.Data(), ec))
  {
    if (dir.is_regular_file())
    {
      i32 const sz = i32(dir.file_size());
      if (sz == 0)
        continue;

      auto const path = dir.path().string();
      if (!path.ends_with(".build.json"))
        continue;

      HANDLE moduleHnd = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      if (moduleHnd != INVALID_HANDLE_VALUE)
      {
        Core::String<char> text(sz + 1, '\0');
        if (ReadFile(moduleHnd, text.Data(), text.AllocSize(), NULL, NULL))
        {
          Module module;
          if (TryParse(path.c_str(), text, module) && !modules.TryEmplace(module.Name.CalculateHash(), std::move(module)))
            std::cerr << "Couldn't insert module " << path << " because it already exists\n";
        }
        else
        {
          std::cerr << "Couldn't read module content of " << path << "\n";
        }
      }
      else if (moduleHnd == INVALID_HANDLE_VALUE)
      {
        std::cerr << "Couldn't open file " << path << "\n";
      }
    }
  }
  return modules;
}
