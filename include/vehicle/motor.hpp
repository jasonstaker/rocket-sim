#pragma once

#include <string>
#include <vector>

namespace vehicle {

class Motor {
 public:
  // Parses .eng file and builds all internal arrays
  explicit Motor(const std::string& eng_file_path);

  // Returns thrust in newtons at time t. 0 after burnout
  double thrust(double t) const;

  // Returns motor mass in kg at time t. Returns dry mass after burnout
  double mass(double t) const;

  double total_impulse() const { return total_impulse_; }
  double dry_mass() const { return dry_mass_; }
  double burnout_time() const { return burnout_time_; }

 private:
  std::vector<double> times_;
  std::vector<double> thrusts_;
  std::vector<double> cumulative_impulse_;

  double total_impulse_;
  double dry_mass_;
  double propellant_mass_;
  double burnout_time_;
};

}  // namespace vehicle