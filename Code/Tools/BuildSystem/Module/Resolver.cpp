#include "Resolver.h"

bool HasModules(Core::FlatMap<u64, Module> const& modules, StringCollection const& coll)
{
  for (auto const& name : coll)
  {
    if (!modules.Contains(name.CalculateHash()))
      return false;
  }
  return true;
}

bool HasModules(Core::FlatMap<u64, Module> const& modules, ModuleConfiguration const& m)
{
  return HasModules(modules, m.Inherits)
      && HasModules(modules, m.PublicIncludePathModules)
      && HasModules(modules, m.PrivateIncludePathModules)
      && HasModules(modules, m.PublicDependencyPathModules)
      && HasModules(modules, m.PrivateDependencyPathModules);
  ;
}

void RemoveModulesWithMissingDependencies(Core::FlatMap<u64, Module>& modules)
{
  while (true)
  {
    Core::Vector<u64> toRemove;
    for (auto const& [key, module] : modules)
    {
      if (!HasModules(modules, module.DefaultConfig))
      {
        toRemove.EmplaceBack(key);
      }
      else
      {
        for (auto const& config : module.Configurations)
        {
          if (!HasModules(modules, config))
          {
            toRemove.EmplaceBack(key);
            break;
          }
        }
      }
    }
    for (u64 const key : toRemove)
      modules.TryRemove(key);
    if (toRemove.IsEmpty())
      return;
  }
}

Core::Vector<Module> ResolveModules(Core::FlatMap<u64, Module>& modules)
{
  RemoveModulesWithMissingDependencies(modules);
}
