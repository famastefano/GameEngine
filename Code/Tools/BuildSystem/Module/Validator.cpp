#include "Validator.h"

#include "../Helpers.h"

#include <format>

bool HasValidDependency(Core::FlatMap<u64, Module> const& modules, StringView name, StringCollection const& dependesOn)
{
  bool valid = true;
  for (auto const& otherName : dependesOn)
  {
    if (name == otherName)
    {
      std::cerr << "Module inherits from himself.\n";
      valid = false;
    }
    else if (!modules.Contains(otherName.CalculateHash()))
    {
      std::cerr << std::format("Missing inherits module `{}`.", String(otherName).Data());
      valid = false;
    }
  }
  return valid;
}

bool IsValid(Core::FlatMap<u64, Module> const& modules, StringView name, ModuleConfiguration const& c)
{
  if (!c.Parsed)
    return true;

  bool valid  = HasValidDependency(modules, name, c.Inherits);
  valid      &= HasValidDependency(modules, name, c.PublicIncludePathModules);
  valid      &= HasValidDependency(modules, name, c.PrivateIncludePathModules);
  valid      &= HasValidDependency(modules, name, c.PublicDependencyPathModules);
  valid      &= HasValidDependency(modules, name, c.PrivateDependencyPathModules);
  return valid;
}

bool IsValid(Core::FlatMap<u64, Module> const& modules, Module const& m)
{
  std::cout << std::format("Validating module `{}`...\n", String(m.Name).Data());

  bool valid = IsValid(modules, m.Name, m.DefaultConfig);
  for (auto const& config : m.Configurations)
    valid &= IsValid(modules, m.Name, config);
  return valid;
}
