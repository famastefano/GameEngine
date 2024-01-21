#pragma once

#ifndef BUILD_CONFIG_DEBUG
#    define BUILD_CONFIG_DEBUG 0
#endif
#ifndef BUILD_CONFIG_DEVELOPMENT
#    define BUILD_CONFIG_DEVELOPMENT 0
#endif
#ifndef BUILD_CONFIG_TESTING
#    define BUILD_CONFIG_TESTING 0
#endif
#ifndef BUILD_CONFIG_SHIPPING
#    define BUILD_CONFIG_SHIPPING 0
#endif

#if BUILD_CONFIG_DEBUG + BUILD_CONFIG_DEVELOPMENT + BUILD_CONFIG_TESTING + BUILD_CONFIG_SHIPPING != 1
#    error Exactly one of [BUILD_CONFIG_DEBUG BUILD_CONFIG_DEVELOPMENT BUILD_CONFIG_TESTING BUILD_CONFIG_SHIPPING] should be defined to be 1
#endif

/*
	Customizable build options. Defaults are defined below depending on the build configuration.

	ENABLE_ASSERT     Compiles into the executable the following macros:
                      assert(condition)
                      assertf(condition, "format", params)
                      assertNoEntry()
                      assertNoReentry()
                      assertNoRecursion()

	ENABLE_EXPECT     Compiles into the executable the following macros:
                      expect(condition)
                      expectf(condition, "format", params)
                      expectAlways(condition)
                      expectAlwaysf(condition, "format", params)

    ENABLE_VERIFY     Compiles into the executable the following macros:
                      verify(condition)
                      verifyf(condition, "format", params)
                      
	ENABLE_LOGGING    Compiles into the executable the following macros:
                      LOG(Logger, "format", params)
*/

#if BUILD_CONFIG_DEBUG
#    ifndef ENABLE_ASSERT
#        define ENABLE_ASSERT 1
#    endif
#    ifndef ENABLE_EXPECT
#        define ENABLE_EXPECT 1
#    endif
#    ifndef ENABLE_VERIFY
#        define ENABLE_VERIFY 1
#    endif
#    ifndef ENABLE_LOGGING
#        define ENABLE_LOGGING 1
#    endif
#    ifndef ENABLE_TESTS
#        define ENABLE_TESTS 1
#    endif
#elif BUILD_CONFIG_DEVELOPMENT
#    ifndef ENABLE_ASSERT
#        define ENABLE_ASSERT 1
#    endif
#    ifndef ENABLE_EXPECT
#        define ENABLE_EXPECT 1
#    endif
#    ifndef ENABLE_VERIFY
#        define ENABLE_VERIFY 1
#    endif
#    ifndef ENABLE_LOGGING
#        define ENABLE_LOGGING 1
#    endif
#    ifndef ENABLE_TESTS
#        define ENABLE_TESTS 1
#    endif
#elif BUILD_CONFIG_TESTING
#    ifndef ENABLE_ASSERT
#        define ENABLE_ASSERT 1
#    endif
#    ifndef ENABLE_EXPECT
#        define ENABLE_EXPECT 1
#    endif
#    ifndef ENABLE_VERIFY
#        define ENABLE_VERIFY 1
#    endif
#    ifndef ENABLE_LOGGING
#        define ENABLE_LOGGING 1
#    endif
#    ifndef ENABLE_TESTS
#        define ENABLE_TESTS 0
#    endif
#elif BUILD_CONFIG_SHIPPING
#    ifndef ENABLE_ASSERT_IN_SHIPPING
#        define ENABLE_ASSERT_IN_SHIPPING 0
#    endif
#    ifndef ENABLE_ASSERT
#        define ENABLE_ASSERT ENABLE_ASSERT_IN_SHIPPING
#    endif
#    ifndef ENABLE_EXPECT
#        define ENABLE_EXPECT 0
#    endif
#    ifndef ENABLE_VERIFY
#        define ENABLE_VERIFY 0
#    endif
#    ifndef ENABLE_LOGGING
#        define ENABLE_LOGGING 0
#    endif
#    ifndef ENABLE_TESTS
#        define ENABLE_TESTS 0
#    endif
#else
#    error Exactly one of [BUILD_CONFIG_DEBUG BUILD_CONFIG_DEVELOPMENT BUILD_CONFIG_TESTING BUILD_CONFIG_SHIPPING] should be defined to be 1
#endif
