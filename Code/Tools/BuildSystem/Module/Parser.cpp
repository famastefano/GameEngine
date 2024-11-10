#include "Parser.h"

#include "../Helpers.h"

#include <rapidjson/document.h>
#include <iostream>
#include <format>

char const* TypeToString(rapidjson::Type const t)
{
  using enum rapidjson::Type;
  switch (t)
  {
  case kNullType:
    return "Null";
  case kFalseType:
    return "Bool";
  case kTrueType:
    return "Bool";
  case kObjectType:
    return "Object";
  case kArrayType:
    return "Array";
  case kStringType:
    return "String";
  case kNumberType:
    return "Number";
  }
  return "Unkown";
}

bool TryParseString(rapidjson::Value const& json, char const* field, String& string)
{
  auto const& data = json[field];
  if (data.IsString())
  {
    string.Assign(StringView(data.GetString(), data.GetStringLength()));
    return true;
  }
  std::cerr << std::format(
      "Invalid type for field `{}`. Expected `{}`, actual `{}`\n",
      field,
      TypeToString(rapidjson::kStringType),
      TypeToString(data.GetType()));
  return false;
}

bool TryParseStringCollection(rapidjson::Value const& json, char const* field, StringCollection& coll)
{
  auto const& data = json[field];
  if (data.IsArray())
  {
    bool valid = true;
    i32  i     = 0;
    for (auto const& item : data.GetArray())
    {
      if (item.IsString())
      {
        coll.EmplaceBack(StringView(item.GetString(), item.GetStringLength()));
      }
      else
      {
        std::cerr << std::format(
            "Invalid type for item #{} of field `{}`. Expected `{}`, actual `{}`\n",
            i,
            field,
            TypeToString(rapidjson::kStringType),
            TypeToString(item.GetType()));
        valid = false;
      }
      ++i;
    }
    return valid;
  }
  std::cerr << std::format(
      "Invalid type for field `{}`. Expected `{}`, actual `{}`\n",
      field,
      TypeToString(rapidjson::kArrayType),
      TypeToString(data.GetType()));
  return false;
}

bool TryParseBool(rapidjson::Value const& json, char const* field, bool& flag)
{
  auto const& data = json[field];
  if (data.IsBool())
  {
    flag = data.GetBool();
    return true;
  }
  std::cerr << std::format(
      "Invalid type for field `{}`. Expected `{}`, actual `{}`\n",
      field,
      TypeToString(rapidjson::kFalseType),
      TypeToString(data.GetType()));
  return false;
}

bool TryParse(rapidjson::Value const& json, ModuleConfiguration& c)
{
  bool validJson = true;
  c.Parsed = true;

  if (json.HasMember("Name"))
    validJson = TryParseString(json, "Name", c.Name);

  if (json.HasMember("Type"))
  {
    using enum ModuleConfiguration::ConfigurationType;

    String type;
    if (TryParseString(json, "Type", type))
    {
      if (type == "ConfigOnly")
        c.Type = ConfigOnly;
      else if (type == "HeaderOnlyLibrary")
        c.Type = HeaderOnlyLibrary;
      else if (type == "SharedLibrary")
        c.Type = SharedLibrary;
      else if (type == "StaticLibrary")
        c.Type = StaticLibrary;
      else if (type == "Executable")
        c.Type = Executable;
      else
      {
        std::cerr << "`Type` invalid value: `" << type << "`\n";
        validJson = false;
      }
    }
    else
    {
      validJson = false;
    }
  }

  if (json.HasMember("Inherits"))
    validJson = TryParseStringCollection(json, "Inherits", c.Inherits);

  if (json.HasMember("PublicIncludePathModules"))
    validJson = TryParseStringCollection(json, "PublicIncludePathModules", c.PublicIncludePathModules);

  if (json.HasMember("PrivateIncludePathModules"))
    validJson = TryParseStringCollection(json, "PrivateIncludePathModules", c.PrivateIncludePathModules);

  if (json.HasMember("PublicDependencyPathModules"))
    validJson = TryParseStringCollection(json, "PublicDependencyPathModules", c.PublicDependencyPathModules);

  if (json.HasMember("PrivateDependencyPathModules"))
    validJson = TryParseStringCollection(json, "PrivateDependencyPathModules", c.PrivateDependencyPathModules);

  if (json.HasMember("PublicIncludePath"))
    validJson = TryParseString(json, "PublicIncludePath", c.PublicIncludePath);

  if (json.HasMember("PrivateIncludePath"))
    validJson = TryParseString(json, "PrivateIncludePath", c.PrivateIncludePath);

  if (json.HasMember("Sources"))
    validJson = TryParseStringCollection(json, "Sources", c.Sources);

  if (json.HasMember("PublicDefinitions"))
    validJson = TryParseStringCollection(json, "PublicDefinitions", c.PublicDefinitions);

  if (json.HasMember("PrivateDefinitions"))
    validJson = TryParseStringCollection(json, "PrivateDefinitions", c.PrivateDefinitions);

  if (json.HasMember("SystemLibraryPaths"))
    validJson = TryParseStringCollection(json, "SystemLibraryPaths", c.SystemLibraryPaths);

  if (json.HasMember("SystemLibraries"))
    validJson = TryParseStringCollection(json, "SystemLibraries", c.SystemLibraries);

  if (json.HasMember("DisableOptimizations"))
    validJson = TryParseBool(json, "DisableOptimizations", c.DisableOptimizations);

  if (json.HasMember("DisableUnityBuild"))
    validJson = TryParseBool(json, "DisableUnityBuild", c.DisableUnityBuild);

  return validJson;
}

bool TryParse(StringView path, StringView text, Module& m)
{
  std::cout << "Parsing " << path.Data() << "\n";

  rapidjson::Document json;
  json.Parse(text.Data());

  if (!json.IsObject())
  {
    std::cerr << "Module isn't a JSON object.\n";
    return false;
  }

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
    if (!configs.IsArray())
    {
      std::cerr << "Field Configurations isn't an array.\n";
      return false;
    }
    for (auto const& config : configs.GetArray())
    {
      ModuleConfiguration c;
      if (!TryParse(config, c))
        return false;
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