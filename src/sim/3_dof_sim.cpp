#include <Eigen/Dense>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "dynamics/environment.hpp"
#include "math/rk4_integrator.hpp"
#include "vehicle/motor.hpp"

enum StateIdx { X = 0, Y = 1, Z = 2, dX = 3, dY = 4, dZ = 5 };

vehicle::Motor engine = vehicle::Motor("config/AeroTech_J415W.eng");
dynamics::Environment env;

double payload_mass = 2.0;

// 1D so the only number we care about is Z velocity
Eigen::Vector<double, 6> state_derivative(const Eigen::Vector<double, 6>& state,
                                          double t) {
  Eigen::Vector<double, 6> derivative;

  double thrust_acceleration =
      (engine.thrust(t) / (engine.mass(t) + payload_mass));
  double gravity_acceleration = env.gravity(state[Z]);

  derivative[X] = 0.0;
  derivative[Y] = 0.0;
  derivative[Z] = state[dZ];
  derivative[dX] = 0.0;
  derivative[dY] = 0.0;
  derivative[dZ] = thrust_acceleration - gravity_acceleration;

  return derivative;
}

int main(int argc, char* argv[]) {
  const std::string output_path = (argc > 1) ? argv[1] : "flight.csv";

  std::ofstream csv(output_path);
  if (!csv) {
    std::cerr << "error: could not open '" << output_path << "' for writing\n";
    return 1;
  }

  csv << "t_s,altitude_m,velocity_ms\n";
  csv << std::fixed << std::setprecision(6);

  Eigen::Vector<double, 6> state(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

  double t = 0.0;
  double dt = 0.01;

  Eigen::Vector<double, 6> prev_state;

  do {
    csv << t << "," << state[Z] << "," << state[dZ] << "\n";
    prev_state = state;
    state = math::rk4Step(state, t, dt, state_derivative);
    t += dt;
  } while (state[dZ] > 0.0);

  // the loop exits after the step that crossed apogee, so the crossing state
  // itself is still unwritten; emit it so the trace ends at apogee
  csv << t << "," << state[Z] << "," << state[dZ] << "\n";

  // diagnostics go to stderr to keep stdout/the data file uncontaminated
  std::cerr << "apogee: "
            << ((state[Z] < prev_state[Z]) ? prev_state[Z] : state[Z])
            << " m at t = " << t << " s\n";
  std::cerr << "wrote " << output_path << "\n";

  return 0;
}
