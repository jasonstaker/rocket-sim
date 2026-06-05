#include <gtest/gtest.h>

#include "vehicle/motor.hpp"

// Should succeed when the file exists
TEST(MOTOR, ReadSuccess) {
  EXPECT_NO_THROW(vehicle::Motor motor("../config/AeroTech_J415W.eng"));
}

// Should succeed when the file doesn't exist
TEST(MOTOR, ReadFailure) {
  EXPECT_THROW(vehicle::Motor motor("../config/AeroTech_J415.eng"),
               std::runtime_error);
}

// Checks the exact values for the thrust and mass
TEST(MOTOR, CheckParsedValues) {
  vehicle::Motor motor("../config/AeroTech_J415W.eng");

  // Thrust exact values
  EXPECT_EQ(motor.thrust(0.0), 0.0);
  EXPECT_EQ(motor.thrust(1.512), 512.126);
  EXPECT_EQ(motor.thrust(2.699), 178.288);
  EXPECT_EQ(motor.thrust(3.358), 0.000);

  // Mass exact values
  EXPECT_EQ(motor.mass(0.0), 1.15718);
  EXPECT_NEAR(motor.mass(motor.burnout_time()), 0.47084, 1e-4);

  // Total impulse, dry mass, burnout time
  EXPECT_NEAR(motor.total_impulse(), 1290.54, 1e-2);
  EXPECT_NEAR(motor.dry_mass(), 0.47084, 1e-4);
  EXPECT_EQ(motor.burnout_time(), 3.358);
}

// Checks the interpolated values for thrust and mass
TEST(MOTOR, CheckInterpolatedValues) {
  vehicle::Motor motor("../config/AeroTech_J415W.eng");

  // Interpolated between (1.381, 537.331) and (1.512, 512.126)
  double expected_thrust_1400 =
      ((512.126 - 537.331) / (1.512 - 1.381)) * (1.400 - 1.381) + 537.331;
  EXPECT_NEAR(motor.thrust(1.400), expected_thrust_1400, 1e-6);

  // Interpolated between (2.567, 297.571) and (2.699, 178.288)
  double expected_thrust_2600 =
      ((178.288 - 297.571) / (2.699 - 2.567)) * (2.600 - 2.567) + 297.571;
  EXPECT_NEAR(motor.thrust(2.600), expected_thrust_2600, 1e-6);

  // Mass mid-burn should be strictly between initial and dry mass
  double mid_mass = motor.mass(1.679);
  EXPECT_GT(mid_mass, motor.dry_mass());
  EXPECT_LT(mid_mass, 1.15718);

  // Mass should be monotonically decreasing during burn
  EXPECT_GT(motor.mass(0.5), motor.mass(1.0));
  EXPECT_GT(motor.mass(1.0), motor.mass(2.0));
  EXPECT_GT(motor.mass(2.0), motor.mass(3.0));
}

// Checks edge cases for thrust and mass
TEST(MOTOR, CheckEdgeValues) {
  vehicle::Motor motor("../config/AeroTech_J415W.eng");

  // Before ignition
  EXPECT_NEAR(motor.thrust(-1.0), 0.0, 1e-9);
  EXPECT_NEAR(motor.mass(-1.0), 1.15718, 1e-6);

  // Well after burnout
  EXPECT_NEAR(motor.thrust(999.0), 0.0, 1e-9);
  EXPECT_NEAR(motor.mass(999.0), motor.dry_mass(), 1e-9);

  // Mass after burnout stays constant
  EXPECT_EQ(motor.mass(10.0), motor.mass(100.0));
  EXPECT_EQ(motor.mass(100.0), motor.mass(1000.0));

  // Thrust after burnout stays zero
  EXPECT_EQ(motor.thrust(10.0), motor.thrust(100.0));
  EXPECT_EQ(motor.thrust(10.0), 0.0);

  // At exact burnout time
  EXPECT_NEAR(motor.thrust(motor.burnout_time()), 0.0, 1e-6);
  EXPECT_NEAR(motor.mass(motor.burnout_time()), motor.dry_mass(), 1e-4);
}