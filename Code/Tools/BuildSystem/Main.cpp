#include "ModuleDefinition.h"

#include <Core/Container/FlatMap.h>
#include <Core/Definitions.h>
#include <Windows.h>
#include <format>
#include <iostream>
#include <rapidjson/document.h>

Core::FlatMap<u64, ModuleDefinition> FindAllModules(Core::StringView<char> path);

std::ostream& operator<<(std::ostream& out, Core::String<char> const& s)
{
  if (s.IsEmpty())
    return out;
  return out << s.Data();
}

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

  auto modules = FindAllModules(argv[1]);
  std::cout << std::format("Found {} modules.\n", modules.Values().Size());
  for (auto const& module : modules.Values())
  {
    std::cout << module.Name << " | " << module.AbsoluteModuleFilePath << "\n";
  }
}

bool TryParse(StringView path, StringView text, ModuleDefinition& m);

Core::FlatMap<u64, ModuleDefinition> FindAllModules(Core::StringView<char> path)
{
  Core::FlatMap<u64, ModuleDefinition> modules;

  Core::String<char> baseDir(path);
  auto const         searchPattern = baseDir + "\\*.build.json";

  WIN32_FIND_DATAA findData{};
  HANDLE           searchHnd = FindFirstFileA(searchPattern.Data(), &findData);
  while (searchHnd != INVALID_HANDLE_VALUE)
  {
    auto const fullPath  = baseDir + "\\" + findData.cFileName;
    HANDLE     moduleHnd = CreateFileA(fullPath.Data(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (moduleHnd != INVALID_HANDLE_VALUE && findData.nFileSizeLow)
    {
      Core::String<char> text(findData.nFileSizeLow + 1, '\0');
      if (ReadFile(moduleHnd, text.Data(), text.AllocSize(), NULL, NULL))
      {
        ModuleDefinition module;
        if (TryParse(fullPath, text, module) && !modules.TryEmplace(module.Name.CalculateHash(), std::move(module)))
          std::cerr << "Couldn't insert module " << fullPath << " because it already exists\n";
      }
      else
      {
        std::cerr << "Couldn't read module content of " << fullPath << "\n";
      }
    }
    else if (moduleHnd == INVALID_HANDLE_VALUE)
    {
      std::cerr << "Couldn't open file " << fullPath << "\n";
    }
    if (!FindNextFileA(searchHnd, &findData))
      break;
  }
  FindClose(searchHnd);
  return modules;
}

#define GE_JSON_ERROR(text)    \
  {                            \
    std::cerr << text << "\n"; \
    return false;              \
  }

#define GE_JSON_CHECK(cnd, text) \
  if (!(cnd))                    \
  {                              \
    std::cerr << text << "\n";   \
    return false;                \
  }

bool TryParse(rapidjson::Value const& json, ModuleConfiguration& c)
{
  return false;
}

bool TryParse(StringView path, StringView text, ModuleDefinition& m)
{
  std::cout << "Parsing " << path.Data() << "\n";

  rapidjson::Document json;
  json.Parse(text.Data());

  GE_JSON_CHECK(json.IsObject(), "Module isn't a JSON object.");

  if (json.HasMember("DefaultConfig"))
  {
    std::cout << "Parsing DefaultConfig...\n";
    if (!TryParse(json["DefaultConfig"], m.DefaultConfig))
      return false;
  }

  if (json.HasMember("Configurations"))
  {
    std::cout << "Parsing Configurations...\n";
    auto const& configs = json["Configurations"];
    GE_JSON_CHECK(configs.IsArray(), "Field Configurations isn't an array.");
    for (auto const& config : configs.GetArray())
    {
      ModuleConfiguration c;
      if (TryParse(config, c))
        m.Configurations.EmplaceBack(std::move(c));
    }
  }

  constexpr auto NotFound = Core::StringView<char>::NotFound;

  m.AbsoluteModuleFilePath = path;
  i32 nameStart            = m.AbsoluteModuleFilePath.AsView().FindLastOf('\\');
  nameStart                = nameStart != NotFound ? nameStart : 0;

  i32 nameEnd = m.AbsoluteModuleFilePath.AsView().Find('.', nameStart);
  nameEnd     = nameEnd != NotFound ? nameEnd : m.AbsoluteModuleFilePath.Size();
  m.Name      = m.AbsoluteModuleFilePath.AsView().SubStr(nameStart + 1, nameEnd - nameStart - 1);

  return true;
}

#undef GE_JSON_ERROR
#undef GE_JSON_CHECK
