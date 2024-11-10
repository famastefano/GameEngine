#pragma once

#include <Core/Container/String.h>

using String           = Core::String<char>;
using StringView       = Core::StringView<char>;
using StringCollection = Core::Vector<Core::String<char>>;

struct ModuleConfiguration
{
  enum class ConfigurationType
  {
    ConfigOnly,
    HeaderOnlyLibrary,
    SharedLibrary,
    StaticLibrary,
    Executable,
  };

  // If it was part of the JSON or not
  bool Parsed = false;

  // Configuration's name.
  // All of those will then allow the IDE/Build System to know which config to apply.
  String            Name;
  ConfigurationType Type;

  // List of modules we inherit the definition from.
  StringCollection Inherits;

  StringCollection PublicIncludePathModules;
  StringCollection PrivateIncludePathModules;
  StringCollection PublicDependencyPathModules;
  StringCollection PrivateDependencyPathModules;

  // Path, relative to module definition file, added to include paths of this module and propagates to our users.
  String PublicIncludePath;

  // Path, relative to module definition file, added to include paths of this module only.
  String PrivateIncludePath;

  // List of files, relative to module definition file, that will be added to the project.
  StringCollection Sources;

  // Preprocessor defines that are added to this module and will propagate to our users.
  StringCollection PublicDefinitions;

  // Preprocessor defines that are added to this module only.
  StringCollection PrivateDefinitions;

  // Libraries we need to link against, like d3dcompiler.lib etc.
  // Only valid for this module, won't propagate to others.
  StringCollection SystemLibraryPaths;
  StringCollection SystemLibraries;

  bool DisableOptimizations = false;
  bool DisableUnityBuild    = false;
};

struct Module
{
  // Filled when discovering and parsing the module definition files.
  String     AbsoluteModuleFilePath;
  StringView Name;
  //---

  // Optional. Allows you to define shared configuration for
  ModuleConfiguration DefaultConfig;

  Core::Vector<ModuleConfiguration> Configurations;
};
