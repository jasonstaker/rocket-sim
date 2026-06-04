#pragma once

// the reason I employ RK4 over something like euler is because RK4 uses
// weighted averages across 4 slopes compared to euler's 1, making it much more
// numerically accurate and stable, especially since the task demands for it

namespace math {

// Runge-Kutta numerical integration step for generic state vectors and
// functions
template <typename State, typename DerivativeFunc>
State rk4Step(const State& state, double t, double dt, DerivativeFunc f) {
  State k1 = f(state, t);
  State k2 = f(state + k1 * (dt / 2.0), t + (dt / 2.0));
  State k3 = f(state + k2 * (dt / 2.0), t + (dt / 2.0));
  State k4 = f(state + k3 * dt, t + dt);

  return state + (k1 + 2.0 * k2 + 2.0 * k3 + k4) * (dt / 6.0);
}

}  // namespace math