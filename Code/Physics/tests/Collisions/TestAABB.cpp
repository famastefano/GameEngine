#include <Physics/Collisions/AABB.h>
#include <UnitTest/UnitTest.h>

UNIT_TEST_SUITE(AABB)
{
  using Math::Vec2Di;
  using Physics::AABB;

  UNIT_TEST(DefaultCtor_HasNoSize)
  {
    AABB box;

    bool const zeroCenter = box.Center_ == Vec2Di{0, 0};
    UNIT_TEST_REQUIRE(zeroCenter);

    bool const zeroExtents = box.Extents_ == Vec2Di{0, 0};
    UNIT_TEST_REQUIRE(zeroExtents);
  }
  UNIT_TEST(Contains_Point_IfSamePointAsCenter)
  {
    AABB box({5, 5}, 3);
    UNIT_TEST_REQUIRE(box.Contains(box.Center_));
  }
  UNIT_TEST(Contains_Point_InsideItsArea)
  {
    AABB box({1, 1}, 3);
    for (i32 x = box.Center_.X() - box.Extents_.X(); x < box.Center_.X() + box.Extents_.X(); ++x)
    {
      for (i32 y = box.Center_.Y() - box.Extents_.Y(); y < box.Center_.Y() + box.Extents_.Y(); ++y)
      {
        UNIT_TEST_REQUIRE(box.Contains(Vec2Di{x, y}));
      }
    }
  }
  UNIT_TEST(Contains_Point_OutsideItsArea)
  {
    AABB box({0, 0}, 10);
    UNIT_TEST_REQUIRE_FALSE(box.Contains(Vec2Di{-11, -11}));
    UNIT_TEST_REQUIRE_FALSE(box.Contains(Vec2Di{11, -11}));
    UNIT_TEST_REQUIRE_FALSE(box.Contains(Vec2Di{-11, 11}));
    UNIT_TEST_REQUIRE_FALSE(box.Contains(Vec2Di{11, 11}));
  }
  UNIT_TEST(Contains_AABB_SameCenterAndExtents)
  {
    AABB boxA({5, 5}, 10);
    AABB boxB = boxA;
    UNIT_TEST_REQUIRE(boxA.Contains(boxB));
    UNIT_TEST_REQUIRE(boxB.Contains(boxA));
  }
  UNIT_TEST(Contains_AABB_SmallerAABB)
  {
    AABB bigger({7, 7}, 20);
    AABB smaller({4, 4}, 5);
    UNIT_TEST_REQUIRE(bigger.Contains(smaller));
    UNIT_TEST_REQUIRE(smaller.Contains(bigger));
  }
  UNIT_TEST(Intersect_PartialOverlapping)
  {
    AABB boxA({0, 0}, 5);
    AABB boxB({0, 0}, {17, 3});
    UNIT_TEST_REQUIRE_FALSE(boxA.Contains(boxB));
    UNIT_TEST_REQUIRE_FALSE(boxB.Contains(boxA));
  }
  UNIT_TEST(InsideInTopRight)
  {
    AABB bigger({3, 4}, 5);
    AABB smaller({6, 7}, 2);
    UNIT_TEST_REQUIRE(bigger.Contains(smaller));
    UNIT_TEST_REQUIRE(smaller.Contains(bigger));
  }
}
