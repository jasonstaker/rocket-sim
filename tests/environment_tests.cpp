#include <gtest/gtest.h>

#include "dynamics/environment.hpp"

// Tests known US Standard Atmosphere reference densities at layer boundaries
// and interiors
TEST(ENVIRONMENT, DensityKnownReferenceValues) {
  dynamics::Environment env;

  EXPECT_NEAR(env.density(0.0), 1.2250, 1e-3);
  EXPECT_NEAR(env.density(5000.0), 0.7364, 1e-3);
  EXPECT_NEAR(env.density(11000.0), 0.3639, 1e-3);
  EXPECT_NEAR(env.density(15000.0), 0.1948, 1e-3);
  EXPECT_NEAR(env.density(20000.0), 0.0889, 1e-3);
  EXPECT_NEAR(env.density(30000.0), 0.0184, 1e-3);
}

// Checks that density decreases as we increase in altitude, monotonically
TEST(ENVIRONMENT, DensityMonotonicDecrease) {
  dynamics::Environment env;

  for (double altitude = 0; altitude < 59500.0; altitude += 500.0) {
    EXPECT_GT(env.density(altitude), env.density(altitude + 500.0));
  }
}

// Tests the inverse-square value formula for correct values at different altitudes
TEST(ENVIRONMENT, GravityKnownReferenceValues) {
  dynamics::Environment env;

  EXPECT_NEAR(env.gravity(0.0), 9.80665, 1e-5);
  EXPECT_NEAR(env.gravity(10000.0), 9.7759, 1e-3);
  EXPECT_NEAR(env.gravity(100000.0), 9.5052, 1e-3);
}

// Checks that gravity decreases as we increase in altitude, monotonically
TEST(ENVIRONMENT, GravityMonotonicDecrease) {
  dynamics::Environment env;

  for (double altitude = 0.0; altitude < 100000.0; altitude += 500.0) {
    EXPECT_GT(env.gravity(altitude), env.gravity(altitude + 500.0));
  }
}