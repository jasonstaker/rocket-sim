#include <gtest/gtest.h>

#include <Eigen/Dense>

#include "math/rk4_integrator.hpp"

// We use a problem with a known solution, so we can compare the
// exact analytical solution against our approximations
Eigen::Vector2d harmonicOscillator(const Eigen::Vector2d& state,
                                   double /* t */) {
  Eigen::Vector2d derivative;

  derivative[0] = state[1];
  derivative[1] = -state[0];

  return derivative;
}

// Simple test using a known analytical solution for the harmonic oscillator
TEST(RK4, HarmonicOscillatorBasic) {
  Eigen::Vector2d state;
  state << 1.0, 0.0;

  double t = 0.0;
  double dt = 0.01;

  for (int i = 0; i < 1000; ++i) {
    state = math::rk4Step(state, t, dt, harmonicOscillator);
    t += dt;

    EXPECT_NEAR(state.x(), std::cos(t), 1e-5);
    EXPECT_NEAR(state.y(), -std::sin(t), 1e-5);
  }
}

// Confirm 4th-order accuracy: halving step size should reduce global error
// by approximately 16x (2^4). A ratio clearly in [12, 20] confirms RK4
// order; ~4x would indicate RK2, ~2x would indicate Euler.
TEST(RK4, HarmonicOscillatorHalfStepError) {
  Eigen::Vector2d stateOriginal;
  stateOriginal << 1.0, 0.0;

  double t = 0.0;
  double dt = 0.01;

  for (int i = 0; i < 1000; ++i) {
    stateOriginal = math::rk4Step(stateOriginal, t, dt, harmonicOscillator);
    t += dt;
  }

  Eigen::Vector2d stateHalf;
  stateHalf << 1.0, 0.0;

  t = 0.0;
  dt = 0.005;

  for (int i = 0; i < 2000; ++i) {
    stateHalf = math::rk4Step(stateHalf, t, dt, harmonicOscillator);
    t += dt;
  }

  const double endTime = 10.0;

  double originalErrorX = std::abs(stateOriginal.x() - std::cos(endTime));
  double originalErrorY = std::abs(stateOriginal.y() + std::sin(endTime));
  double halfErrorX = std::abs(stateHalf.x() - std::cos(endTime));
  double halfErrorY = std::abs(stateHalf.y() + std::sin(endTime));

  EXPECT_GT(originalErrorX / halfErrorX, 12.0);
  EXPECT_LT(originalErrorX / halfErrorX, 20.0);
  EXPECT_GT(originalErrorY / halfErrorY, 12.0);
  EXPECT_LT(originalErrorY / halfErrorY, 20.0);
}