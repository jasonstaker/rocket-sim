#include "dynamics/environment.hpp"

namespace dynamics {

static const AtmosphereLayer kLayers[] = {
    // {base_altitude (m), base_temp (K), base_pressure (Pa), lapse_rate (K/m)}
    {0.0, 288.15, 101325.00, -0.0065},    // Troposphere
    {11000.0, 216.65, 22632.10, 0.0000},  // Tropopause (isothermal)
    {20000.0, 216.65, 5474.89, 0.0010},   // Stratosphere lower
    {32000.0, 228.65, 868.02, 0.0028},    // Stratosphere upper
    {47000.0, 270.65, 110.91, 0.0000},    // Stratopause (isothermal)
    {51000.0, 270.65, 66.94, -0.0028},    // Mesosphere lower
    {71000.0, 214.65, 3.96, -0.0020},     // Mesosphere upper
};

static const size_t kNumLayers = sizeof(kLayers) / sizeof(kLayers[0]);

static const double g0 = 9.80665;              // gravity m/s^2
static const double M = 0.0289644;             // kg/mol (molar mass of air)
static const double R = 8.31446;               // J/(mol·K)
static const double kEarthRadius = 6371000.0;  // meters
static const double kGeopotentialRadius =
    6356766.0;  // m (US Std Atmosphere 1976)

double Environment::gravity(double altitude) const {
  return g0 * (kEarthRadius / (kEarthRadius + altitude)) *
         (kEarthRadius / (kEarthRadius + altitude));
}

double Environment::density(double altitude) const {
  // Convert geometric altitude to geopotential height — US Standard Atmosphere
  // 1976 is defined in geopotential height, which accounts for gravity
  // weakening with altitude
  double H = kGeopotentialRadius * altitude / (kGeopotentialRadius + altitude);

  AtmosphereLayer layer = getAtmosphereLayer(H);

  double P;

  double temp = layer.base_temp + layer.lapse_rate * (H - layer.base_altitude);

  if (layer.lapse_rate == 0.0) {
    P = layer.base_pressure *
        exp(-g0 * M * (H - layer.base_altitude) / (R * layer.base_temp));
  } else {
    P = layer.base_pressure *
        pow(layer.base_temp / temp, g0 * M / (R * layer.lapse_rate));
  }

  return P * M / (R * temp);
}

AtmosphereLayer Environment::getAtmosphereLayer(double altitude) const {
  for (int i = (int)kNumLayers - 1; i >= 0; --i) {
    if (altitude >= kLayers[i].base_altitude) {
      return kLayers[i];
    }
  }

  return kLayers[0];
}

}  // namespace dynamics