#pragma once

#include <cmath>

namespace dynamics {

struct AtmosphereLayer {
  double base_altitude;  // meters
  double base_temp;      // Kelvin
  double base_pressure;  // Pascals
  double lapse_rate;     // K/m (0 for isothermal)
};

class Environment {
 public:
  // calculates gravity in m/s^2 using the inverse-square with altitude
  double gravity(double altitude) const;

  // Returns air density in kg/m^3 using the US Standard Atmosphere 1976 model
  double density(double altitude) const;

 private:
  AtmosphereLayer getAtmosphereLayer(double altitude) const;
};
}  // namespace dynamics