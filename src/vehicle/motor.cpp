#include "vehicle/motor.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace vehicle {

Motor::Motor(const std::string& eng_file_path) {
  std::ifstream file(eng_file_path);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open motor file: " + eng_file_path);
  }

  std::string line;
  bool parse_header = true;

  while (std::getline(file, line)) {
    // Skip comments
    if (!line.empty() && line[0] == ';') continue;

    // Skip empty lines
    if (line.empty()) continue;

    std::istringstream iss(line);

    // Parsing the header
    if (parse_header) {
      std::string motor_name, manufacturer;
      int diameter, length;
      double delays, propellant_mass, initial_mass;

      iss >> motor_name >> diameter >> length >> delays >> propellant_mass >>
          initial_mass >> manufacturer;

      propellant_mass_ = propellant_mass;
      dry_mass_ = initial_mass - propellant_mass;

      parse_header = false;
      continue;
    }

    double time, thrust;

    // Parsing the time/thrust pairs
    if (iss >> time >> thrust) {
      times_.push_back(time);
      thrusts_.push_back(thrust);

      size_t idx = times_.size() - 1;

      if (idx == 0) {
        cumulative_impulse_.push_back(0.0);
      } else {
        cumulative_impulse_.push_back(cumulative_impulse_.back() +
                                      0.5 *
                                          (thrusts_[idx] + thrusts_[idx - 1]) *
                                          (times_[idx] - times_[idx - 1]));
      }
    }
  }

  if (times_.empty()) {
    throw std::runtime_error("No data points parsed from: " + eng_file_path);
  }

  burnout_time_ = times_.back();
  total_impulse_ = cumulative_impulse_.back();

  file.close();
}

double Motor::thrust(double t) const {
  if (t > burnout_time_) {
    return 0.0;
  }

  if (t <= times_.front()) return thrusts_.front();
  if (t >= times_.back()) return thrusts_.back();

  size_t lower, upper;

  for (size_t i = 0; i < times_.size(); i++) {
    if (t < times_[i]) {
      lower = i - 1;
      upper = i;
      break;
    }
  }

  double thrust_u = thrusts_[upper];
  double thrust_l = thrusts_[lower];
  double time_u = times_[upper];
  double time_l = times_[lower];

  return ((thrust_u - thrust_l) / (time_u - time_l)) * (t - time_l) + thrust_l;
}

double Motor::mass(double t) const {
  if (t > burnout_time_) {
    return dry_mass_;
  }

  if (t <= times_.front()) return dry_mass_ + propellant_mass_;
  if (t >= times_.back()) return dry_mass_;

  size_t lower, upper;

  for (size_t i = 0; i < times_.size(); i++) {
    if (t < times_[i]) {
      lower = i - 1;
      upper = i;
      break;
    }
  }

  double ci_u = cumulative_impulse_[upper];
  double ci_l = cumulative_impulse_[lower];
  double time_u = times_[upper];
  double time_l = times_[lower];

  double ci_interp = ((ci_u - ci_l) / (time_u - time_l)) * (t - time_l) + ci_l;

  double fraction_burned = ci_interp / total_impulse_;
  return dry_mass_ + propellant_mass_ * (1 - fraction_burned);
}

}  // namespace vehicle