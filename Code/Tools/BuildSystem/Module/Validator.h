#pragma once

#include "Module.h"

#include <Core/Container/FlatMap.h>

bool IsValid(Core::FlatMap<u64, Module> const& modules, Module const& m);