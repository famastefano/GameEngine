#pragma once

#include "Core/LoggerManager.hpp"

#include <catch2/catch_test_macros.hpp>

#define GE_TEST_TRUE(condition) REQUIRE(((condition) && !Core::LoggerManager::isGlobalErrorSet()))
