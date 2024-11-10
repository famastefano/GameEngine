#pragma once

#include "Module.h"
#include <Core/Container/FlatMap.h>

Core::Vector<Module> ResolveModules(Core::FlatMap<u64, Module>& modules);