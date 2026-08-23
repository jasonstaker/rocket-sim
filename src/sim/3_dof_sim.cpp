#include <Eigen/Dense>
#include <iostream>

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

int main() {
  Eigen::Vector<double, 6> state(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

  double t = 0.0;
  double dt = 0.01;

  Eigen::Vector<double, 6> prev_state;

  do {
    std::cout << state[Z] << ", " << state[dZ] << "\n";
    prev_state = state;
    state = math::rk4Step(state, t, dt, state_derivative);
    t += dt;
  } while (state[dZ] > 0.0);

  std::cout << "\n" << t << "\n";
  std::cout << ((state[Z] < prev_state[Z]) ? prev_state[Z] : state[Z]) << "\n";

  return 0;
}